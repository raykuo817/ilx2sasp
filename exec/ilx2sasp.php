<?php
    function create_playlist(array $playlist, array $tracks, $m3udir, &$filelists, $musicFolder)
    {
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

					$location = str_replace($musicFolder, '../../', $location);
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
                echo 'Update     : '.$fullfile."\n";
                file_put_contents($fullfile, $m3u);
            }
            else
            {
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
    } //create_playlist


    ///////////////////
    $itunesxml = '';            //itunes library xml
    $plistdir = '';               //audio station playlists directory
    $ignore_playlists = array();

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

