# Chess C

Chess in your terminal!

## Building in Windows

You can build in Windows by either using WSL or MinGW(or its alternatives).

For WSL you can just use the Linux methods.

If you want it to be an `.exe`, you need to install MinGW or its alternatives.

Here's a website explaining how to install MinGW and GCC: [https://dev.to/gamegods3/how-to-install-gcc-in-windows-10-the-easier-way-422j](https://dev.to/gamegods3/how-to-install-gcc-in-windows-10-the-easier-way-422j)

After installing gcc on your Windows machine you can just run the `build.cmd` file in the project folder.

After this you should be seeing a `chess.exe` in the project folder.

## Building in MacOS or Linux

First run `chmod +x build.sh` to let your OS trait the file as an executable.

Then run `./build.sh`, after this the program should compile to `./chess` in the project folder.