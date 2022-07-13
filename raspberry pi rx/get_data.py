import board
import busio
from digitalio import DigitalInOut
import adafruit_rfm9x

CS = DigitalInOut(board.D7)
RESET = DigitalInOut(board.D22)
spi = busio.SPI(board.D11, MOSI = board.D10, MISO = board.D9)

# Output file
output = "mairq_data.txt"

try:
   rfm9x = adafruit_rfm9x.RFM9x(spi, CS, RESET, 433.8)
except RuntimeError as error:
   print("RFM9x error: ", error)

rfm9x.tx_power = 12

while True:
   packet = None
   packet = rfm9x.receive()

   if packet is not None:
      try:
        packet = packet.split(b"\x00")[0]
        packet_text = str(packet, "utf-8")

        with open(output, "a") as file:
          file.write(packet_text + "\n")

        response = bytes("OK\0", "utf-8")
        rfm9x.send(response)
      except:
         pass
