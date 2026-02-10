# paraphrase (uploading in progress)
A cross-platform c++ paraphrase tool built with qt5 and libllama.
## Table of Contents
- [Quick Start](#quick-start)
- [Info](#guide)
- [Dev](#dev)
- [Other](#other)
<a id="quick-start"></a>
## Quick Start
**Default HotKey: Alt+v**
[Download Latest Release](https://github.com/mise-dev/paraphrase/releases/latest)

<a id="guide"></a>
## Info
**Basic app info**
Stack: c++, qt5, libllama.
This app was created as a privacy tool: it should prevent write stile tracing.
You can build it by yourself or run a pre-build portable version.
**Config.conf** provides some ricing options.
```bash
# hotkey
hotkey=Alt+v
# window settings
width=480
height=360
background_img=img.png
overwrite_xy=0
x=0
y=0
# font AARRGGBB
font_color=aaffffff
# model (only llama-3.2-1b-instruct-q8_0.gguf tested)
model=llama-3.2-1b-instruct-q8_0.gguf
```
By default window appears in the screen center.


<a id="dev"></a>
## Dev
If you find any bugs, don't mind reporting.
- https://github.com/mise-dev/paraphrase/issues

If you have any suggestions/ideas, share with the dev team.
 - https://github.com/mise-dev/paraphrase/discussions

 If you want to help the development:
 - bc1qtmezw0sfmq6pz5gvsml6qlv7f89g7u0ae3p69p
 - bc1q8fwscqtprwkauk24f6qtm2mayjh0epsfn3wagm
 - (btc)

<a id="other"></a>
## Other
 ### Qt Licensing Notice

This application uses **Qt5** under the **GNU Lesser General Public License (LGPL) version 3**.

#### Important Information for Users:
- This application dynamically links to Qt5 libraries
- You have the right to:
  - Replace the Qt5 libraries with modified versions
  - Reverse-engineer the application for debugging purposes
  - Receive the source code for the Qt5 libraries used

#### Source Code Availability:
The source code for Qt5 can be obtained from:
- Official Qt repository: https://code.qt.io/cgit/qt/qt5.git/
- Qt website: https://www.qt.io/download

#### Additional License Information:
This application does **not** use Qt under a commercial license. All Qt components used are available under LGPL v3 or compatible licenses.
### Ai Model used
 - https://huggingface.co/hugging-quants/Llama-3.2-1B-Instruct-Q8_0-GGUF

You can test other models if you want.

[Back to Top](#table-of-contents)
<a id="config"></a>
