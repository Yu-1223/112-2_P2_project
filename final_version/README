# Interactive Fiction Engine (IFE)

## 簡介
互動式小說引擎（Interactive Fiction Engine, IFE）是一個讓使用者能夠遊玩他們自己的互動式小說遊戲的程式。該專案包括以下主要部分：
- 定義人類可讀的文字劇本檔格式
- 實作遊戲引擎
- 支援圖片顯示及處理
- 提供範例遊戲劇本

## 劇本檔格式
劇本檔格式基於 TOML 設計，允許遊戲創作者以結構化的方式編寫和組織故事情節、角色互動及場景變化。以下是一個範例劇本檔 script.toml 的結構：

[[dialogue.home.start]]
    character = "system"
    text = "It's a sunny morning, and you have just woken up, sitting comfortably at home."
    background = "./image/start.jpeg"
[[dialogue.home.start]]
    character = "system"
    text = "The sunlight streams through the window, and today seems like a perfect day for an adventure."
[[dialogue.home.start]]
    character = "system"
    text = "You look around and find the house peaceful, but it seems there's something you need to explore."  
[[dialogue.home.start]]
    character = "system"
    text = "You find an adorable Banana Cat behind the sofa. It looks a bit uneasy and needs your help."     

[[dialogue.home.found_banana_cat]]
    character = "You"
    text = "You: Banana Cat, what's wrong? Do you need my help?"
    background = "./image/home.jpeg"
[[dialogue.home.found_banana_cat]]
    character = "Banana_Cat"
    text = "Banana Cat: Meow... I don't feel well..."
[[dialogue.home.found_banana_cat]]
    character = "You"
    text = "You: What happened? Are you sick?"
[[dialogue.home.found_banana_cat]]
    character = "Banana_Cat"
    text = "Banana Cat: Meow... I don't know, I just feel off."
[[dialogue.home.found_banana_cat]]
    character = "You"
    text = "You: Let me see, maybe I can help."
[[dialogue.home.found_banana_cat]]
    character = "Banana_Cat"
    text = "Banana Cat: Meow... thank you."
[[dialogue.home.found_banana_cat]]
    character = "You"
    text = "You: Don't worry, I'll help you."
    [[dialogue.home.found_banana_cat.prompt]]
    text = "In a corner of the house, you find a soothing ball. This ball seems to be able to help Banana Cat relax."
    [[dialogue.home.found_banana_cat.prompt]]
        text = "Pick up"
        next = "home.pick_up_smoothing_ball"
    [[dialogue.home.found_banana_cat.prompt]]
        text = "Ignore"
        next = "home.ignore_smoothing_ball"


## 系統架構
系統主要模組包括：
- 劇本解析器（`toml.c`, `toml.h`）
- 遊戲引擎核心（`main.c`）
- 顯示介面（`display_interface.c`, `display_interface.h`）

## 使用方式
### 編譯
首先，請確保您的系統上安裝了必要的編譯工具。
操作系統：Ubuntu 22.04
編譯器：gcc
需要的庫：
ncurses
SDL2
SDL2_mixer

可以使用以下命令安裝必要的庫：
sudo apt-get install libncurses5-dev libncursesw5-dev libsdl2-dev libsdl2-mixer-dev

請使用支援Sixel圖形顯示的終端機，如：
xterm
iTerm2 (macOS)

使用以下命令進行編譯：
make

### 遊戲運行
編譯完成後，使用以下命令開始遊戲：
./main


## 文件結構
專案文件結構如下：
├── Makefile
├── main.c
├── toml.c
├── toml.h
├── display_interface.c
├── display_interface.h
├── script.toml
├── images/
│   └── (images...)
├── audio/
│   └── (audio...)
└── README.md


## 已知問題
- 目前圖片顯示僅支援 Sixel 格式
- 尚未實現遊戲存檔及回復功能

## 貢獻者
- 成員A：核心程式設計與劇本解析
- 成員B：介面設計與顯示功能
- 成員C：範例劇本編寫與測試

## 聯絡方式
如有任何問題或建議，請聯絡 [41044028s@ntnu.edu.tw]
