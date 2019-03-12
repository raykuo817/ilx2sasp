# ilx2sasp for Docker

iTunes Library XML 2 Synology Audio Station Playlists



### 主要功能：

ilx2sasp 是一個執行在Docker上的小工具程式，主要用來將 iTunes playlists 轉成 Synology audio station playlists。



### 說明：

個人習慣在電腦上使用 iTunes 整理音樂，在手機使用 Synology Audio Station (DS Audio) 播放音樂，透過 Synology Drive 同步音樂或是直接將 iTunes Library 設在 NAS 都可以達到 iTunes 與 Audio Station 音樂檔案保持同步，可是有個問題，那就是在 iTunes 建立的音樂播放清單如何讓 Synology Audio Station 使用呢？為了解決這個問題我寫了一個 PHP 小程式 (ilx2sasp – iTunes Library XML 2 Synology Audio Station Playlists) 來做轉換的工作。

ilx2sasp 可以將 iTunes 音樂播放清單轉成 Audio Station 可用的音樂播放清單，因為 iTunes 本身的原因所以只達到單向同步 (iTunes -> Audio Station) 而無法雙向同步。

這裏簡單說明一下 ilx2sasp 的工作原理，ilx2sasp 只是單純去解析 iTunes Library XML 中的播放清單，跟據 XML 內容將其轉成 Aduio Station 的播放列表格式 m3u 檔案。



### iTunes Library 檔案結構：

iTunes 使用 iTunes 資料庫 (iTunes Library) 檔案和 iTunes Media 檔案夾，來整理及存放音樂和其他媒體。您的 iTunes 資料庫檔案會追蹤您加入 iTunes 的媒體、您整理媒體的方式，以及播放列表、播放次數和歌曲喜好程度等其他資訊。

在 Windows 中 iTunes Library 預設位置在 `c:\Users\user_name\Music\` ，也就是 `『音樂』` 資料夾。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/iTunes-Library.jpg)



在 iTunes 資料夾中，`iTunes Library.itl` 一個資料庫檔案，裡面存放您資料庫中的歌曲資訊，以及您建立的播放列表，某些與歌曲相關的資料也儲存在此檔案中。

`iTunes Media` 資料夾則用來存放歌曲、電影、鈴聲等多媒體檔案。

`iTunes Music Library.xml (或 iTunes Library.xml)` 則由 iTune 產生提供給第三方應用程式使用，其中包括歌曲、播放列表相關資訊。

ilx2sasp 就是利用這個 XML 檔來將 iTunes 播放列表轉成 Audio Station 可用的播放列表 (*.m3u) 檔。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/A2_13_091918_014304_PM.jpg)



預設情況下 iTunes 不會建立 iTunes Music Library.xml 檔案，必需在 iTunes `『徧好設定』`中選取`『與其他應用程式共享 iTunes 資料庫 XML`，當 iTunes Library 有更新時 XML 才會同步更新。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/A2_11_091918_015840_PM.jpg)



要注意的是 XML 檔名可有能是 `『iTunes Music Library.xml』` 或是 `『iTunes Library.xml』`，iTunes 只會使用其中一個，如果兩個 xml 檔案都存在，可以將它刪除讓 iTunes 重新產生。



要使用 ilx2sasp 轉換歌曲播放列表，應該選取`『徧好設定』`中的 `『保持 iTunes Media 資料夾的整理狀態』`與 `『歌曲加入資料庫時，將其檔案複製到 iTunes Media 資料夾』`，然後執行`『統整資料庫』`的動作，這可以讓 iTunes 重新整理所有的媒體檔案，並依不同的媒體類別 (書籍、歌曲、電影、鈴聲) 將檔案複製到 \iTunes\iTunes Media\ 下的 Books、Music、Movie、Tones 資料夾，這樣 ilx2sasp 輸出的 m3u 播放列表中的歌曲路徑才不會錯誤。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/A2_12_091918_021516_PM.jpg)



![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Screenshot_091918_035731_PM.jpg)



### 讓 iTunes 管理的歌曲與 Audio Station 中的歌曲保持一致：

要讓 iTunes 管理的歌曲與 Audio Station 中的歌曲保持一致，基本上有三個方法。

1. 使用 Synology Drive 來同步 NAS music 共用資料夾 (Audio Station 存放歌曲的資料夾) 與電腦上的 iTunes Media 或 iTunes Media\music 資料夾 。
2. 將 iTunes Library 搬到 NAS music 共用資料夾。
3. 變更 Windows 『音樂』資料夾位置，改到 NAS music 共用資料夾。因為 iTunes Library 預設就是放在 Windows 『音樂』資料夾中，所以變更『音樂』資料夾位置等同變更 iTunes Library 位置。



##### 方法 1：使用 Synology Drive 同步音樂

這個就不提了，依 Drive 軟體進行設定即可。



##### 方法 2、3：需要設定網路磁碟機將 NAS music 音樂資料夾掛載至電腦

下面以將 NAS music 資料夾掛載到磁碟機 `m:\` 為例，其中 `192.168.1.88` 是 NAS IP。

Windows 10 連線網路磁碟機：開啟『檔案總管』，左窗格選取 『本機』。然後，在『電腦』索引標籤中，選取『連線網路磁碟機』。
![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/A2_14_091918_062059_PM.jpg)



為目的地資料夾指定一個`『磁碟機代號』`，然後在資料夾欄位輸入 `\\NAS IP\music` ，按一下完成來結束網路磁碟機連線的工作。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/D-1.png)

 

Window 7 連線網路磁碟機：開啟『檔案總管』，左窗格選取『電腦』，按一下上方的『連線網路磁碟機』。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Untitled_Clipping_091918_062703_PM.jpg)



為目的地資料夾指定一個`『磁碟機代號』`，然後在資料夾欄位輸入 `\\NAS IP\music` ，按一下完成來結束網路磁碟機連線的工作。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Untitled_Clipping_091918_062846_PM.jpg)



##### 方法 2：將 iTunes Library 搬到 NAS music 共用資料夾

1. 如果 iTunes 正在執行，請結束 iTunes。
2. 使用『檔案總管』將 iTunes Library (也就是 `c:\Users\user_name\Music\`下的 `iTunes` 資料夾) 搬到掛載 NAS music 資料夾的磁碟機 `m:\`。
3. 按住 `Shift` 鍵執行 iTunes，點選『`選擇資料庫`』。
4. 選擇 `m:\iTunes\iTunes Library.itl` 。![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/8D.png)



##### 方法 3：變更 Windows 『音樂』資料夾位置

1. 如果 iTunes 正在執行，請結束 iTunes。
2. 開啟『檔案總管』，在左窗格『音樂』使用滑鼠『右鍵』，選『內容』。
3. 切換至『位置』頁籤，在點選『移動』。
4. 選擇掛載 NAS music 資料夾的磁碟機代號 `m:\` 。 

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/A2_16_092018_100814_AM.jpg)



### Synology Audio Station 播放列表：

Audio Station 播放清單分成『個人播放清單』與『共同播放清單』。

『個人播放清單』存放在路徑 `/usr/syno/etc/preference/user_name/playlists/` ，無法使用 File Station 存取。

『共同播放清單』則存放在 music 共用資料夾下 playlists 子資料夾 (`/music/playlists/`) ，可以使用 File Station 或透過 SMB / FTP 等方式存取，如果增加或刪除存放資料夾中的 m3u 檔，在 Audio Station 中也會看到相對應的變化。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Screenshot_030216_051711_PM.jpg)



### ilx2sasp – iTunes Library XML 2 Synology Audio Station Playlists：

ilx2sasp 會解析 iTunes Library XML 內容，將 XML 中的播放列表資訊轉成 Audio Station 的播放清單 m3u 格式。ilx2sasp 以 PHP7.2 寫成，可以在 Windows / Linux 下執行。



ilx2sasp 是單向同步 (iTunes -> m3u playlists)，當 iTunes 播放列表有變動、刪除、或新增播放列表，ilx2sasp 轉出的 m3u 播放列表也會有相應該的更新。



ilx2sasp 執行需要 PHP 7.2，要使用 ftp 功能在 PHP.ini 加入下面兩行
`extension_dir = "./ext"`
`extension=ftp`



下載 ilx2sasp 解開 zip 裏面有幾個檔案：

| ilx2sasp.php ilx2sasp.ini-sample PlistParser.inc ftp.php | ilx2sasp.php 是主程式，PlistParser.inc 與 ftp.php  為程式執行需要的副程式。 `ilx2sasp.ini-sample` 為設定檔樣本，程式執行前需先 設定正確內容，並將檔名改為 `ilx2sasp.ini`。 |
| -------------------------------------------------------- | ------------------------------------------------------------ |
|                                                          |                                                              |
| Dockerfile <br />startup.sh                              | 用來 build Docker image 的 Dockerfile，如果想重 build   build Docker image 的話可以參考。 |
| d.sh                                                     | docker 測試用 shell script                                   |



### ilx2sasp.ini 內容：

```
; 要產生 m3u 播放列表的目錄
M3U_DIR = ./playlists

; 指向 iTunes Library XML 檔
; 檔名可能是 "iTunes Music Library.xml" 或 "iTunes Library.xml"
XML = /music/iTunes/iTunes Music Library.xml

; 1 = 只執行一次產生 m3u 播放列表就結束程式
; 0 = 持續執行, 每當 XML 有變動時, 跟據 XML 內容自動產生 m3u 播放列表
RUN_ONCE = 0

; 每隔多少秒查詢一次 XML 是否有變動 (當 RUN_ONCE = 0 時)
INTERVAL = 60

;
REPLACE_XML_MUSIC_FOLDER = ../iTunes/iTunes Media/
REMOVE_PREFIX_PATH =

; 1 = 使用 FTP 方法上傳 M3U 播放列表, 0 = 不使用 FTP
FTP = 0

; 1 = 被動動模, 0 = 主動模式
FTP_PASV = 1

; FPT Server host IP
; "DOCKER_HOST_IP" 表示由程式自動偵測 Server IP, 當程式在 Docker 中執行時有效
FTP_HOST_NAME = DOCKER_HOST_IP

; FTP Port
FTP_PORT = 21

; FTP username and password
FTP_USER = user
FTP_PASSWORD = password

; FTP 中 m3u 所在目錄 (要上傳的目錄)
FTP_M3U_DIR = /music/playlists

; 要略過不處理的播放列表, 可以加在下面 [IGNORE] 中, 
; 或是在 iTunes 中將播放列表改名第一個字加入!(金嘆號)
[IGNORE]
0 = 要略過不處理的播放列表1
1 = 要略過不處理的播放列表2
2 = 要略過不處理的播放列表3
```

ilx2sasp 有兩種執行模式，一種是只執行一次就結束程式，另一種則會讓 ilx2sasp 進入無窮迴圈直到按 Ctrl-C 中斷程式執行，`RUN_ONCE` 參數就是用來設定這個行為模式。

`INTERVAL` 則用來設定在無窮迴圈模式中每隔多少秒查詢一次 iTunes Library XML 檔是否有變動。

ilx2sasp 有支援 FTP，設好 IP 與帳號密碼並開啟 NAS 的 FTP 功能，產生的播放列表 m3u 檔案會自動上傳到 NAS 中。

如果 iTunes 中有不想讓 ilx2sasp 處理的播放列表，可直接新增在` [IGNORE]` 區域，或是在 iTunes 中將播放列表改名第一個字加入 `!` (金嘆號) ，ilx2sasp 便會略過這些播放列表不處理。



例如在 Windwos 下執行，將 Windows `『音樂』`目錄設在 NAS music 資料夾，並掛載成網路磁碟機 `m:\` ， ini 可以這樣設定：

```
M3U_DIR = m:\playlists 
XML = m:\iTunes\iTunes Music Library.xml 
RUN_ONCE = 0
FTP FTP = 0
```



### 在 Docker 中執行：

直接在 docker hub 下載 [dockerhub raykuo/ilx2sasp](https://cloud.docker.com/repository/docker/raykuo/ilx2sasp) 映像檔，完成後使用 File Station 建立一個名為 ilx2sasp 的資料夾，然後點選 『ilx2sasp映像檔』 -> 『佈署』-> 『進階設定』-> 『儲存空間』-> 『新增資料夾』-> 掛載 music 與 ilx2sasp 資料夾 -> 『套用』 -> 『下一步』-> 『套用』，完成佈署。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Screenshot_092518_013657_PM.jpg)



首次執行會出現 『Docker 容器：ilx2sasp 意外停止』的錯誤訊息，這是正常情況因為 ini 還沒設定的關係。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Screenshot_092518_015445_PM.jpg)



使用 File Station 切到掛載的 ilx2sasp 目錄，可以看到多了一個 ilx2sasp.ini 檔，開啟並編輯檔案。

![img](https://github.com/raykuo817/ilx2sasp/tree/docker/img/Screenshot_092518_020032_PM.jpg)



因為我是將 Windows 『音樂』資料夾位置，改到 NAS music 共用資料夾，所以 ini 可以這樣設。

```
M3U_DIR = ./playlists
XML = /music/iTunes/iTunes Music Library.xml

RUN_ONCE = 0
INTERVAL = 60

REPLACE_XML_MUSIC_FOLDER = ../iTunes/iTunes Media/
REMOVE_PREFIX_PATH =

FTP = 1
FTP_PASV = 1
FTP_HOST_NAME = DOCKER_HOST_IP
FTP_PORT = 21
FTP_USER = user
FTP_PASSWORD = password
FTP_M3U_DIR = /music/playlists
```



然後再次啟動容器 ilx2sasp，ilx2sasp 便可以在 Docker 正常執行，而且每當 iTunes 播放列表有變更時，ilx2sasp 就會更新 /music/playlists 資料夾中的播放列表 m3u 檔案。



有一點要特別說明，如果：

```
M3U_DIR = /music/playlists
FPT = 0
```

ilx2sasp 會直接將 m3u 輸出到 /music/playlists 目錄中。

但這樣會有一個問題，那就是由 Docker 容器產生的檔案，DSM 系統會『不知道』有更新。也就是說 ilx2sasp 容器雖然正確的在 playlists 資料夾產生 m3u 播放列表，使用 File Station 也可以看到這些播放列表，可是 Audio Station 或 Cloud Sync 等套件不會知道 playlists 資料夾下有新增加的播放列表，這就造成雖然播放列表存在可是當使用 DS Audio 或 Audio Station 時卻看不到。

只有通過『正常』方法新增的播放列表 DSM 系統才會知道檔案有更新，例如 FTP 、 File Station 或者是檔案總管等。

