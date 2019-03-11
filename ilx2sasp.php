<?php

	// Windows 下使用 PHP FTP function
	// 編輯 PHP.ini, 加入下面兩列
	//   extension_dir = "./ext"
	//   extension=ftp
    include("ftp.php");

    include("PlistParser.inc");		//https://github.com/jsjohnst/php_class_lib

	function load_default()
	{
		$cfg['M3U_DIR']= "./playlists";
		$cfg['XML'] = "/music/iTunes/iTunes Music Library.xml";
		if (!file_exists($cfg['XML']))
			$cfg['XML'] = "/music/iTunes/iTunes Library.xml";
		$cfg['RUN_ONCE'] = 0;
		$cfg['INTERVAL'] = 60;
		$cfg['IGNORE']= array();
		$cfg['PL_NEW']= array();
		$cfg['PL_IGNORE']= array();
		$cfg['PL_UPDATED']= array();
		$cfg['PL_NO_CHANGED']= array();
		$cfg['FTP'] = 0;
		$cfg['FTP_PASV'] = 1;
		$cfg['FTP_HOST_NAME'] = '';
		$cfg['FTP_PORT'] = 21;
		$cfg['FTP_USER'] = '';
		$cfg['FTP_PASSWORD'] = '';
		$cfg['FTP_M3U_DIR'] = '';
		$cfg['REPLACE_XML_MUSIC_FOLDER'] = '../';
		$cfg['REMOVE_PREFIX_PATH'] = '';
		$cfg['FTP_UPLOAD'] = array();
		$cfg['FTP_DEL'] = array();
		return $cfg;
	}

    function load_config($IniFile)
    {
		$cfg = load_default();

		if (file_exists($IniFile)) {
			$usrcfg = parse_ini_file($IniFile, true, INI_SCANNER_RAW);
		}
		elseif (file_exists(__DIR__.'/'.basename($IniFile))) {
			$usrcfg = parse_ini_file(__DIR__.'/'.basename($IniFile), true, INI_SCANNER_RAW);
		}

		if (isset($usrcfg['M3U_DIR'])) {
			$cfg['M3U_DIR'] = $usrcfg['M3U_DIR'];
			if (!is_dir($cfg['M3U_DIR'])) {
				@mkdir($cfg['M3U_DIR']);
				if (!is_dir($cfg['M3U_DIR'])) {
					echo "Error: Failed to create '".$cfg['M3U_DIR']."' directory.\n";
					return FALSE;
				}
			}
		}
		else {
			if (!is_dir($cfg['M3U_DIR'])) {
				@mkdir($cfg['M3U_DIR']);
				if (!is_dir($cfg['M3U_DIR'])) {
					echo "Error: Failed to create '".$cfg['M3U_DIR']."' directory.\n";
					return FALSE;
				}
			}
		}

		if (isset($usrcfg['XML'])) $cfg['XML'] = $usrcfg['XML'];
		if (!file_exists($cfg['XML'])) {
			echo "Error: Unable to open '".$cfg['XML']."'.\n";
			return FALSE;
		}

		if (isset($usrcfg['RUN_ONCE'])) $cfg['RUN_ONCE'] = (int)$usrcfg['RUN_ONCE'];
		if (isset($usrcfg['INTERVAL'])) $cfg['INTERVAL'] = (int)$usrcfg['INTERVAL'];
		if (isset($usrcfg['IGNORE'])) $cfg['IGNORE'] = $usrcfg['IGNORE'];
		if (isset($usrcfg['FTP'])) $cfg['FTP'] = (int)$usrcfg['FTP'];
		if (isset($usrcfg['FTP_PASV'])) $cfg['FTP_PASV'] = (int)$usrcfg['FTP_PASV'];
		if (isset($usrcfg['FTP_HOST_NAME'])) $cfg['FTP_HOST_NAME'] = $usrcfg['FTP_HOST_NAME'];
		if (isset($usrcfg['FTP_PORT'])) $cfg['FTP_PORT'] = (int)$usrcfg['FTP_PORT'];
		if (isset($usrcfg['FTP_USER'])) $cfg['FTP_USER'] = $usrcfg['FTP_USER'];
		if (isset($usrcfg['FTP_PASSWORD'])) $cfg['FTP_PASSWORD'] = $usrcfg['FTP_PASSWORD'];
		if (isset($usrcfg['FTP_M3U_DIR'])) $cfg['FTP_M3U_DIR'] = $usrcfg['FTP_M3U_DIR'];
		if (isset($usrcfg['REPLACE_XML_MUSIC_FOLDER'])) $cfg['REPLACE_XML_MUSIC_FOLDER'] = $usrcfg['REPLACE_XML_MUSIC_FOLDER'];
		if (isset($usrcfg['REMOVE_PREFIX_PATH'])) $cfg['REMOVE_PREFIX_PATH'] = $usrcfg['REMOVE_PREFIX_PATH'];

		return $cfg;
	}  // load_config


	function clean_filename($filename)
	{
        return preg_replace('/[*|\\\\:"<>?\/]/', "_", $filename);		// *|\:"<>?/
		return $filename;
	}

    function create_m3u(array $playlist, array $tracks, array &$cfg, $musicFolder)
    {
		$replace_xml_music_folder = $cfg['REPLACE_XML_MUSIC_FOLDER'];
		$remove_prefix_path = $cfg['REMOVE_PREFIX_PATH'];

        $m3u = "#EXTM3U\n";
        foreach ($playlist['Playlist Items'] as $item) {
			if (isset($item['Track ID'])) {
				$id = $item['Track ID'];
				$track = $tracks[$id];
				if (isset($track['Location'])) {
					$location = $track['Location'];
					$location = rawurldecode($location);

                    if (substr($location, 0, 17) != "file://localhost/") {
						$location = "file://localhost/".str_replace('\\', '/', $location);
                    }

					$location = str_replace($musicFolder.$remove_prefix_path, $replace_xml_music_folder, $location);
					if (strtolower(substr($location,-4))!=".m4v") {
    					if (substr($location,0,strlen($replace_xml_music_folder))==$replace_xml_music_folder) {
    						$m3u .= $location.PHP_EOL;
    					}
                    }
				}
			}
        }

		$m3u_filename = clean_filename($playlist['Name'].".m3u");
		$fullfile = $cfg['M3U_DIR']."/".$m3u_filename;

		$count = 0;
		if (in_array($playlist['Name'], $cfg['IGNORE'])) {
			if (isset($cfg['PL_IGNORE'])) $count = count($cfg['PL_IGNORE']);
			$cfg['PL_IGNORE'][$count] = $playlist['Name'];
		}
		else {
			$org_m3u = @file_get_contents($fullfile);
			if ($org_m3u != $m3u) {
				if (file_exists($fullfile)) {
					$count = count($cfg['PL_UPDATED']);
					$cfg['PL_UPDATED'][$count] = $playlist['Name'];
				}
				else {
					$count = count($cfg['PL_NEW']);
					$cfg['PL_NEW'][$count] = $playlist['Name'];
				}
				file_put_contents($fullfile, $m3u);
			}
			else
			{
				$count = count($cfg['PL_NO_CHANGED']);
				$cfg['PL_NO_CHANGED'][$count] = $playlist['Name'];
			}
		}
    } //create_m3u

    function parse_xml(&$cfg)
	{
		if ($cfg == FALSE)
			return FALSE;

		$xml = array();
		$parser = new plistParser();
		$xml = $parser->parseFile($cfg['XML']);

		if (!isset($xml["Tracks"])) {
			echo "iTunes Library XML error: No 'Tracks' !!\n";
			return FALSE;
		}
		if (!isset($xml["Playlists"])) {
			echo "iTunes Library XML error: No 'Playlists' !!\n";
			return FALSE;
		}
		if (!isset($xml["Music Folder"])) {
			echo "iTunes Library XML error: No 'Music Folder' !!\n";
			return FALSE;
		}

		$musicFolder = rawurldecode($xml["Music Folder"]);
		$tracks = $xml["Tracks"];
		$playlists = $xml["Playlists"];

		foreach($playlists as $pl) {
			if (substr($pl['Name'], 0, 1) == '!') {
				array_push($cfg['IGNORE'], $pl['Name']);
			}
		}
		$cfg['IGNORE'] = array_values($cfg['IGNORE']);

		foreach($playlists as $pl) {
			if (isset($pl["Master"])) continue;
			if (isset($pl["Distinguished Kind"])) continue;
			if (isset($pl["Playlist Items"])) {
				create_m3u($pl, $tracks, $cfg, $musicFolder);
			}
		}
		return TRUE;
	} //parse_xml

	function del_m3u_files(&$cfg, &$ftp)
	{
		$new_sync = array();
		$old_sync = array();

		$new_sync = array_merge($cfg['PL_NO_CHANGED'], $cfg['PL_UPDATED'], $cfg['PL_NEW']);
		asort($new_sync);
		$new_sync = array_values($new_sync);

		if (file_exists(__DIR__.'/sync.dat')) {
			$old_sync = @file_get_contents(__DIR__.'/sync.dat');
			$old_sync = explode(PHP_EOL, $old_sync);
			$old_sync = array_filter($old_sync, 'strlen');
			asort($old_sync);
			$old_sync = array_values($old_sync);
		}

		if ($new_sync != $old_sync) {
			$tmp = array();
			foreach($new_sync as $key => $pl) {
				$tmp[$key] = $pl.PHP_EOL;
			}
			@file_put_contents(__DIR__.'/sync.dat', $tmp);
		}

		$del = $old_sync;
		foreach ($old_sync as $key => $pl) {
			if (in_array($pl, $new_sync)) {
				unset($del[$key]);
			}
		}
		foreach ($del as $key => $pl) {
			$m3u_filename = clean_filename($pl.".m3u");
			$fullfile = $cfg['M3U_DIR']."/".$m3u_filename;
			if (is_file($fullfile)) {
				unlink($fullfile);
				echo "  Delete playlist: $pl\n";
				$cfg['FTP_DEL'][count($cfg['FTP_DEL'])] = $pl;
			}
		}
	} //del_m3u_files

	///////////////////
	echo "ilx2sasp - iTunes Library XML 2 Synology Audio Station Playlists\n";

	$cfg = load_config('/ilx2sasp/ilx2sasp.ini');		//for Docker

	$ftp = new ftp();

	$lasttime = 0;
	while ($cfg != NULL) {
		clearstatcache();
		if (file_exists($cfg['XML'])) {
			if ($lasttime != filemtime($cfg['XML'])) {
				$lasttime = filemtime($cfg['XML']);
				$cfg = load_config('/ilx2sasp/ilx2sasp.ini');
				if (parse_xml($cfg) != FALSE) {
					echo "========================================\n";
					foreach ($cfg['PL_UPDATED'] as $key => $pl) {
						echo "  Playlist update: $pl\n";
						$cfg['FTP_UPLOAD'][count($cfg['FTP_UPLOAD'])] = $pl;
					}
					foreach ($cfg['PL_NEW'] as $key => $pl) {
						echo "  New playlist: $pl\n";
						$cfg['FTP_UPLOAD'][count($cfg['FTP_UPLOAD'])] = $pl;
					}
					del_m3u_files($cfg, $ftp);

					if (count($cfg['FTP_UPLOAD']) || count($cfg['FTP_DEL']) ) {
						if ($cfg['FTP']) {
							if (strtoupper($cfg['FTP_HOST_NAME']) == "DOCKER_HOST_IP") {
								$cfg['FTP_HOST_NAME'] = exec('/sbin/ip route|awk \'/default/ { print $3 }\'');
							}
							if (!$ftp->connect($cfg['FTP_HOST_NAME'], $cfg['FTP_PORT'], $cfg['FTP_PASV'], $cfg['FTP_USER'], $cfg['FTP_PASSWORD'])) {
								echo "FTP connection/login has failed!\n";
							}
						}

						foreach ($cfg['FTP_UPLOAD'] as $key => $pl) {
							if ($ftp->is_available()) {
								$m3u_filename = clean_filename($pl.".m3u");
								if (!$ftp->upload($cfg['FTP_M3U_DIR'], $cfg['M3U_DIR'], $m3u_filename)) {
									echo "FTP upload of '".$m3u_filename."' as failed!\n";
									$ftp->close();
									break;
								}
							}
						}

						foreach ($cfg['FTP_DEL'] as $key => $pl) {
							if ($ftp->is_available()) {
								$m3u_filename = clean_filename($pl.".m3u");
								if (!$ftp->delete($cfg['FTP_M3U_DIR'], $m3u_filename)) {
									echo "FTP could not delete '".$m3u_filename."'!\n";
									$ftp->close();
									break;
								}
							}
						}

						if ($ftp->is_available()) {
							$ftp->close();
						}
					}

				}
			}
		}

		if ($cfg['RUN_ONCE'])
			break;

		sleep($cfg['INTERVAL']);
	}
?>



