# Animate a rainbow starting dark and getting to full saturation
for b in 15...256 by 64 {
  # cycle through the whole hue rainbow
  for h in 0x600 by 64 {
    # Inner loop gives rainbow hue index.
    # Outer loop gives mix value from black to rainbow.
    pwm(mix(0, hue(h), b))
    # Delay enough to get 60fps
    delay(16)
  }
}

# Same thing, but fade to white instead of from black
for b in 15...256 by 64 {
  # cycle through the whole hue rainbow
  for h in 0x600 by 64 {
    # Mix rainbow towards white
    pwm(mix(0, hue(h), b))
    delay(16)
  }
}

# Blink the status leds in unision 3 times
# and then alternating 3 times.
tx = true
rx = true
for i in 12 {
  # Update the LEDs using variables
  pin(1, tx)
  pin(2, rx)
  # Delay a bit
  delay(250)
  # Invert the variables
  tx = !tx
  rx = !rx
  # Halfway through, make them alternate
  if i == 6 { tx = false }
}

# Then, just for fun, show this is a general purpose programming lang:
# Count down with decreasing delays
for i in 500..0 by 25 {
  delay(i)
}

# And sum the first 10000 integers for the final return value.
sum = 0
for i in 10000 {
  sum = sum + i
}