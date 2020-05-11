package main

import (
	"fmt"
	"log"
	"math"
	"os"
	"os/signal"
	"strings"
	"syscall"
	"time"

	linuxproc "github.com/c9s/goprocinfo/linux"
	"github.com/karalabe/hid"
)

const VENDOR_ID = 0xFC51
const PRODUCT_ID = 0x058
const INTERFACE = 1
const DEBUG = false
const BAR_CHAR = string(0x7F)

const SYSDISK = "sdb"

type Command byte
const(
	SetUp = 0x00
	Clear = 0x01
	SetLine = 0x02
	Present = 0x03
)

type Screen byte
const(
	Master = 0x00
	Slave = 0x01
)

type OLEDController struct {
	device *hid.Device
	columns, rows uint8
}

func (oled *OLEDController) RunPerfInfo(screen Screen) {
	var prevIdle, prevTotal, prevIOTicks uint64
	var prevUptime float64

	for {
		var cpu, mem, swap, disk float64

		stats, err := linuxproc.ReadStat("/proc/stat")
		if err != nil {
			log.Println("Failed to retrieve stat information:", err)
		} else {
			idle := stats.CPUStatAll.Idle + stats.CPUStatAll.IOWait
			nonIdle := stats.CPUStatAll.User + stats.CPUStatAll.Nice + stats.CPUStatAll.System + stats.CPUStatAll.IRQ + stats.CPUStatAll.SoftIRQ + stats.CPUStatAll.Steal
			total := idle + nonIdle

			if prevIdle != 0 && prevTotal != 0 {
				totalDelta := total - prevTotal
				idleDelta := idle - prevIdle
				cpu = math.Max(float64(totalDelta - idleDelta) / float64(totalDelta), 0)
				if DEBUG {
					log.Println("CPU%: ", cpu * 100)
				}
			}

			prevIdle = idle
			prevTotal = total
		}

		meminfo, err := linuxproc.ReadMemInfo("/proc/meminfo")
		if err != nil {
			log.Println("Failed to retrieve meminfo information:", err)
		} else {
			mem = math.Max(float64(meminfo.MemTotal - meminfo.MemAvailable) / float64(meminfo.MemTotal), 0)
			swap = math.Max(float64(meminfo.SwapTotal - meminfo.SwapFree) / float64(meminfo.SwapTotal), 0)
			if DEBUG {
				log.Println("Mem%: ", mem * 100)
				log.Println("Swap%:", swap * 100)
			}
		}

		uptime, err := linuxproc.ReadUptime("/proc/uptime")
		if err != nil {
			log.Println("Failed to retrieve uptime information:", err)
		} else {
			diskStats, err := linuxproc.ReadDiskStats("/proc/diskstats")
			if err != nil {
				log.Println("Failed to retrieve disk stat information for " + SYSDISK + ":", err)
			} else {
				for _, diskStat := range diskStats {
					if diskStat.Name == SYSDISK {
						if (prevIOTicks != 0) {
							disk = math.Max(float64(diskStat.IOTicks - prevIOTicks) / (uptime.Total - prevUptime) / 1000, 0)
							if DEBUG {
								log.Println("Disk%:", disk * 100)
							}
						}
						prevIOTicks = diskStat.IOTicks
						break
					}
				}
			}

			prevUptime = uptime.Total
		}

		barLen := oled.columns - 7
		cpuStr := fmt.Sprintf("CPU%%[%-*s]", barLen, strings.Repeat(BAR_CHAR, int(math.Round(float64(barLen) * cpu))))
		memStr := fmt.Sprintf("Mem%%[%-*s]", barLen, strings.Repeat(BAR_CHAR, int(math.Round(float64(barLen) * mem))))
		swapStr := fmt.Sprintf("Swap[%-*s]", barLen, strings.Repeat(BAR_CHAR, int(math.Round(float64(barLen) * swap))))
		diskStr := fmt.Sprintf("Disk[%-*s]", barLen, strings.Repeat(BAR_CHAR, int(math.Round(float64(barLen) * disk))))
		oled.DrawScreen(screen, []string{cpuStr, memStr, swapStr, diskStr})

		time.Sleep(1 * time.Second)
	}
}

func (oled *OLEDController) DrawScreen(screen Screen, lines []string) {
	for i, line := range lines {
		if i > int(oled.rows) {
			log.Printf("Attempting to draw more rows than the OLED supports: %d/%d\n", i, oled.rows)
			break
		}
		if len(line) > int(oled.columns) {
			log.Printf("Attempting to draw more columns than the OLED supports: %d/%d\n", len(line), oled.columns)
		}
		oled.SendCommand(SetLine, screen, append([]byte{byte(i)}, line...))
		time.Sleep(10 * time.Millisecond)
	}
	oled.SendCommand(Present, screen, nil)
}

func (oled *OLEDController) SendCommand(cmd Command, screen Screen, data []byte) bool {
	buf := make([]byte, 32)

	// Special commands that will bypass VIA, if enabled.
	buf[0] = 0x02
	buf[1] = 0x00

	// Third byte is the command.
	buf[2] = byte(cmd)
	// Fourth byte is the screen index.
	buf[3] = byte(screen)

	if data != nil {
		copy(buf[4:32], data)
	}
	_, err := oled.device.Write(buf)
	if err != nil {
		log.Fatalln("Failed to write to device:", err)
		return false
	}
	if DEBUG {
		log.Println(">", buf[:])
	}

	return true
}

func (oled *OLEDController) ReadResponse() []byte {
	buf := make([]byte, 32)
	size, err := oled.device.Read(buf)
	if err != nil {
		log.Fatalln("Failed to read from device:", err)
		return nil
	}
	if DEBUG {
		log.Println("<", buf[:size])
	}
	if buf[0] != 0 {
		log.Printf("Command 0x%02X failed with error 0x%02X.", buf[2], buf[0])
		return nil
	}
	return buf[4:]
}

func (oled *OLEDController) Run() {
	// Start by setting up
	oled.SendCommand(SetUp, Master, nil)
	resp := oled.ReadResponse()
	if resp == nil {
		log.Fatalln("Set up failed. Bailing.")
		return
	}

	oled.columns = resp[0]
	oled.rows = resp[1]

	if DEBUG {
		log.Printf("OLED size %dx%d\n", oled.columns, oled.rows)
	}
	if oled.columns < 1 || oled.rows < 1 {
		log.Println("Failed to get screen size from set up.")
		oled.device.Close()
		return
	}

	// Read loop
	done := make(chan bool, 1)
	go func() {
		for {
			select {
			case <-done:
				return
			default:
				oled.ReadResponse()
			}
		}
	}()

	// Clean up on exit
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM)
	go func() {
		<- sigs
		oled.SendCommand(Clear, Master, nil)
		done <- true
		oled.SendCommand(Clear, Slave, nil)
		oled.device.Close()
		os.Exit(0)
	}()

	oled.DrawScreen(Master, []string{"Layer: %l", "You look great today!"})
	oled.RunPerfInfo(Slave)
}

func main() {
	log.SetPrefix("oled_controller ")
	log.Println("Started.")

	for {
		for _, devInfo := range hid.Enumerate(VENDOR_ID, PRODUCT_ID) {
			// FIXME: This check is weak, and will match a keyboard without raw HID enabled...
			//        Usage and UsagePage are only supported on Windows/Mac.
			if devInfo.Interface == INTERFACE {
				log.Println("Found device at:", devInfo.Path, devInfo.Usage, devInfo.UsagePage)
				device, err := devInfo.Open()
				if err != nil {
					log.Fatalln("Failed to open device:", err)
					os.Exit(1)
				}

				oled := OLEDController{device: device}
				oled.Run()
			}
		}
		time.Sleep(2 * time.Second)
	}
}
