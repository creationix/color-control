# MCU "Rainbow Test"

This is a test of the lights on the Daplie board, to test that the MCU has been flashed correctly, and that the MCU light commands are working as intended.<br><br>

**Step 0: Pre-Work:**<br>

For this test, you will need:
  * A pre-configured Daplie dev board (should already be flashed using the Windows `STM32-ST-LINK Utility`)<br><br>

**Step 1: Connect to the Daplie board:**<br>

You can connect to the Daplie board either through a serial connection or through SSH (recommended method).

To connect via serial connection, attach the serial cable to your Mac and to the Daplie board.  Open a terminal window and run the following:

```
my_tty=$(ls /dev/tty.usbserial* | grep tty)
echo $my_tty
screen -xRS daplie $my_tty 115200
```

To connect via SSH, you will need to determine what IP and/or hostname the board is.  If you did not set the board up yourself, 
you will need to contact a team member to get the IP and hostname (SSH) information.  When you have that, you will need to SSH as `root`.  
There is no password on the test boards.<br><br>

**Step 2:  Run the test:**<br>

Once connected to the Daplie board, run the `reboot` command.  When the Daplie board reboots, the lights around the outside perimeter of 
the Daplie board should turn `green` and then fade out.

Run this command to verify that the correct TTY is set up:<br>

```
ls /dev | grep ttyACM0
```
<br>
Run `git clone https://git.daplie.com/Daplie/mcu-controller.git --recursive` to create the test environment.  

When the clone finishes, `cd` into the directory `~/mcu-controller`.

Run `make run`

If the configuration is correct, you should now see the lights on the outer rim of the Daplie board go through a series of colors.  <br><br>

NOTE:  If you get the following, but don't get a rainbow pulse:

```
cat sample.script | ./compile | ./exec
```

run the command `make clean` and then do the `make run` again.  It should work after a clean rebuild.  <br><br>

**Legacy Notes (from previous README version)**<br><br>

- Install `tcc` on linux (`sudo apt-get install tcc`)
- Source the aliases in bash to get `compile` and `exec` commands. (`. alias.sh`)
- Run the test script through the compiler and the emulator. (`cat test | compile | exec`)
