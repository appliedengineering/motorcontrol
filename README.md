# motorcontrol
Brushed DC motor controller and code based around BTN8982 and Arduino Uno

---

### PCB todo:
* Layout
* Determine input/output connectors and component values

---

<a href="https://www.infineon.com/dgdl/Infineon-Motor_Control_Shield_with_BTN8982TA_for_Arduino-UM-v01_00-EN.pdf?fileId=5546d4624ca27d02014cb20b89867eed" target="_blank">Infineon Shield Manual</a>

<a href="https://www.infineon.com/dgdl/Infineon-BTN8982TA-DS-v01_00-EN.pdf?fileId=db3a30433fa9412f013fbe32289b7c17" target="_blank">BTN8982 Datasheet</a>

<a href="https://www.infineon.com/dgdl/Infineon-NovalithIC_BTN89x0-x2-AN-v00_04-EN.pdf?fileId=db3a30433fa9412f013fc8d88e3d430a" target="_blank">BTN8982 Application Guide</a>

---

### Notes

Pulse duration (µs) = (duty/100 * 1/freq) * 10^6

| Duty (%)    | PWM Freq (Hz) | Time (µs) | Prescale | OCR2A   | OCR2B  |
|-------------|---------------|-----------|----------|---------|--------|
| 1           | 1250          | 8         | /64      | 200 - 1 | 2 - 1  |
| 2           | 2500          | 8         | /32      | 200 - 1 | 4 - 1  |
| 3           | 2500          | 12        | /32      | 200 - 1 | 6 - 1  |
| 4           | 5000          | 8         | /32      | 100 - 1 | 4 - 1  |
| 5           | 5000          | 10        | /32      | 100 - 1 | 5 - 1  |
| 6           | 5000          | 12        | /32      | 100 - 1 | 6 - 1  |
| 7           | 5000          | 14        | /32      | 100 - 1 | 7 - 1  |
| 8           | 10000         | 8         | /8       | 200 - 1 | 16 - 1 |
| 9           | 10000         | 9         | /8       | 200 - 1 | 18 - 1 |
| 10          | 10000         | 10        | /8       | 200 - 1 | 20 - 1 |
| 11          | 10000         | 11        | /8       | 200 - 1 | 22 - 1 |
| 12          | 10000         | 12        | /8       | 200 - 1 | 24 - 1 |
| 13          | 10000         | 13        | /8       | 200 - 1 | 26 - 1 |
| 14          | 13333         | 10.5      | /8       | 150 - 1 | 21 - 1 |
| 15          | 16666         | 9         | /8       | 120 - 1 | 18 - 1 |
| 16          | 20000         | 8         | /8       | 100 - 1 | 16 - 1 |
