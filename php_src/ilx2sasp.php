<?php

    function create_playlist(array $playlist, array $tracks, $m3udir, &$filelists, $musicFolder)
    {
//$log = new Logging();
//$log->lfile('logfile.txt');
//
//$log->lwrite("=====================================================");
//$log->lwrite($musicFolder);
//$log->lwrite("=====================================================");

        $file = $playlist['Name'].".m3u";
        $file = iconv("UTF-8", "BIG5", $file);
        $file = preg_replace("/[<>:?\\/\"]/", "_", $file);

        $count_track = 0;
        $m3u = "#EXTM3U\n";
        foreach ($playlist['Playlist Items'] as $item) {
			if (isset($item['Track ID'])) {
				$id = $item['Track ID'];
				$track = $tracks[$id];
				if (isset($track['Location'])) {
					$location = $track['Location'];
					$location = urldecode($location);

                    if (substr($location, 0, 17) != "file://localhost/") {
						$location = "file://localhost/".str_replace('\\', '/', $location);
                    }

//$log->lwrite($count_track." ".$location);
					$location = str_replace($musicFolder, '../../', $location);
//$log->lwrite($count_track." ".$location);
					if (strtolower(substr($location,-4))!=".m4v") {
    					if (substr($location,0,6)=='../../') {
    						$m3u .= $location."\n";
    						$count_track++;
    					}
                    }
				}
			}
        }

        if ($count_track) {
            $fullfile = $m3udir.$file;
            $fullfile = preg_replace("/\\\\/", "/", $fullfile);

            $org_m3u = @file_get_contents($fullfile);
            if ($org_m3u != $m3u) {
//$log->lwrite('Update     : '.$fullfile);
                echo 'Update     : '.$fullfile."\n";
                file_put_contents($fullfile, $m3u);
            }
            else
            {
//$log->lwrite('No updates : '.$fullfile);
                echo 'No updates : '.$fullfile."\n";
            }
        }

        foreach ($filelists as $key => $f) {
            if ($f=="." || $f=="..") {
                unset($filelists[$key]);
            }
            if ($f == $file) {
                unset($filelists[$key]);
            }
        }

//$log->lclose();
    } //create_playlist

	/**
	 * Logging class:
	 * - contains lfile, lwrite and lclose public methods
	 * - lfile sets path and name of log file
	 * - lwrite writes message to the log file (and implicitly opens log file)
	 * - lclose closes log file
	 * - first call of lwrite method will open log file implicitly
	 * - message is written with the following format: [d/M/Y:H:i:s] (script name) message
	 */
	class Logging {
		// declare log file and file pointer as private properties
		private $log_file, $fp;
		// set log file (path and name)
		public function lfile($path) {
			$this->log_file = $path;
		}
		// write message to the log file
		public function lwrite($message) {
			// if file pointer doesn't exist, then open log file
			if (!is_resource($this->fp)) {
				$this->lopen();
			}
			// define script name
			$script_name = pathinfo($_SERVER['PHP_SELF'], PATHINFO_FILENAME);
			// define current time and suppress E_WARNING if using the system TZ settings
			// (don't forget to set the INI setting date.timezone)
			date_default_timezone_set('Asia/Taipei');
			$time = @date('[Y/m/d:H:i:s]');
			// write current time, script name and message to the log file
			fwrite($this->fp, "$time ($script_name) $message" . PHP_EOL);
		}
		// close log file (it's always a good idea to close a file when you're done with it)
		public function lclose() {
			fclose($this->fp);
		}
		// open log file (private method)
		private function lopen() {
			// in case of Windows set default log file
			if (strtoupper(substr(PHP_OS, 0, 3)) === 'WIN') {
				$log_file_default = 'c:/php/logfile.txt';
			}
			// set default log file for Linux and other systems
			else {
				$log_file_default = '/tmp/logfile.txt';
			}
			// define log file from lfile method or use previously set default
			$lfile = $this->log_file ? $this->log_file : $log_file_default;
			// open log file for writing only and place file pointer at the end of the file
			// (if the file does not exist, try to create it)
			$this->fp = fopen($lfile, 'a') or exit("Can't open $lfile!");
		}
	}
    ///////////////////
    $itunesxml = '';            //itunes library xml
    $plistdir = '';               //audio station playlists directory
    $ignore_playlists = array();

//$log = new Logging();
//$log->lfile('logrun.txt');
//$log->lwrite("==============================");
//$log->lclose();

	if (file_exists(dirname(__FILE__)."/ilx2sasp.ini")) {
		$ini = parse_ini_file(dirname(__FILE__)."/ilx2sasp.ini", true, INI_SCANNER_RAW);
		$config = $ini['CONFIG'];

		$itunesxml = $config['ITUNES_LIBRARY_XML'];
		$plistdir = $config['AUDIO_STATION_PLAYLISTS_DIR'];
		if (substr($plistdir,-1) != "\\") {
			if (substr($plistdir,-1) != "/") {
				$plistdir .= "/";
			}
		}

        if (isset($ini['IGNORE'])) {
            $ignore_playlists = $ini['IGNORE'];
    		foreach ($ignore_playlists as $key => $val)
    		{
    			$ignore_playlists[$key] = iconv("BIG5", "UTF-8", $val);
    		}
        }

	}
	else {
		$mode = '';
		foreach($argv as $a) {
			if ((strtolower($a) == "-i") || (strtolower($a) == "-o") || (strtolower($a) == "-d")) {
				$mode = strtolower($a);
			}
			else if ($mode == "-i") {
				$itunesxml = $a;
			}
			else if ($mode == "-o") {
				$plistdir = $a;
				if (substr($plistdir,-1) == "\"") {
					$plistdir = substr($plistdir,0, -1);
				}
				if (substr($plistdir,-1) != "\\") {
					if (substr($plistdir,-1) != "/") {
						$plistdir .= "/";
					}
				}
			}
			else if ($mode == "-d") {
				$a = iconv("BIG5", "UTF-8", $a);
				$ignore_playlists[count($ignore_playlists)]=$a;
			}
		}
	}

    if ($itunesxml == '')
    {
        if (file_exists(dirname(__FILE__)."/iTunes Library.itl"))
        {
            if (file_exists(dirname(__FILE__)."/iTunes Music Library.xml"))
            {
                $itunesxml = dirname(__FILE__)."/iTunes Music Library.xml";
            }
            else if (file_exists(dirname(__FILE__)."/iTunes Library.xml"))
            {
                $itunesxml = dirname(__FILE__)."/iTunes Library.xml";
            }

            $plistdir = dirname(__FILE__)."/iTunes Media/Music/Playlists/";
        }
    }

    if ($itunesxml == '' || $plistdir == '') {
		echo "iTunes Library XML 2 Synology Audio Station playlists Tools \n";
        echo "ilx2sasp.php [-i XML_FILE] [-o OUT_DIR] [-d IGNORE_PLAYLIST]\n";
        echo "   XML_FILE        : iTunes XML FILE\n";
        echo "   OUT_DIR         : Audio Station Playlists Dir\n";
        echo "   IGNORE_PLAYLISTS  : Ignore playplists \n";
        return;
    }

    $itunesxml = preg_replace("/\\\\/", "/", $itunesxml);
    $plistdir = preg_replace("/\\\\/", "/", $plistdir);
    if (!is_file($itunesxml)) {
        echo "Error: Unknown input file ".$itunesxml."\n";
        return;
    }
    if (!is_dir($plistdir)) {
        echo "Error: Unknown output directory ".$plistdir."\n";
        return;
    }

    ///////////////////
    include("PlistParser.inc");		//https://github.com/jsjohnst/php_class_lib
    $parser = new plistParser();
    $plist = $parser->parseFile($itunesxml);

//var_dump($plist);

    if (!isset($plist["Tracks"])) {
        echo "iTunes XML error: No Tracks !!\n";
        return;
    }
    if (!isset($plist["Playlists"])) {
        echo "iTunes XML error: No Playlists !!\n";
        return;
    }
    if (!isset($plist["Music Folder"])) {
        echo "iTunes XML error: No Music Folder !!\n";
        return;
    }

	$tracks = $plist["Tracks"];
	$playlists = $plist["Playlists"];

    $musicFolder = $plist["Music Folder"].'Music/';
    $musicFolder = urldecode($plist["Music Folder"]).'Music/';

    $m3udir = $plistdir."iTunes/";
    if (!is_dir($m3udir)) {
        @mkdir($m3udir);
    }
    $filelists = scandir($m3udir);
	foreach($playlists as $pl){
        if (isset($pl["Master"])) {
        }
        else if (isset($pl["Distinguished Kind"])) {
        }
        else if (isset($pl["Playlist Items"])) {
            if (!in_array($pl['Name'], $ignore_playlists)) {
                create_playlist($pl, $tracks, $m3udir, $filelists, $musicFolder);
            }
        }
	}

    foreach($filelists as $f) {
        $file = $plistdir.$f;
        if (is_file($file)) {
            unlink($file);
            echo "Delete : " . $file . "\n";
        }
    }

?>

