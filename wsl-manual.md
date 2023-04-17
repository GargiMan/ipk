On WSL2 host firewall has to be disabled or port has to be opened to be able to access from LAN.
On WSL2 bridge with LAN can be done by running the following command on the WSL2 host(powershell or cmd):

<code>netsh interface portproxy add v4tov4 listenport=[port] listenaddress=0.0.0.0 connectport=[port] connectaddress=[IP of WSL2 network adapter]</code>

IP of WSL2 network adapter can be obtained on the WSL2 by <code>hostname -I</code>, server needs to be running on the same IP adress (not localhost).

From LAN network WSL2 can be accessed by IP of the WSL2 host and port.
Only works for IPv4.

[Accessing network applications with WSL](https://learn.microsoft.com/en-us/windows/wsl/networking#accessing-linux-networking-apps-from-windows-localhost)

Not tested solution:
[Expose-WSL](https://github.com/icflorescu/expose-wsl)
