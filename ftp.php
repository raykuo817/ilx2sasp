<?php

class ftp
{
	var $conn;
	var $available = false;

    public function connect($host_name, $port, $pasv, $user, $pwd)
	{
		if ($this->is_available()) {
			$this->close();
		}

		$this->conn = @ftp_connect($host_name, $port);
		if (@ftp_login($this->conn, $user, $pwd)) {
			ftp_pasv($this->conn, $pasv);
			$this->available = true;
			return true;
		}
		return false;
	}

    public function close()
	{
		@ftp_close($this->conn);
		$this->available = false;;
	}

	public function is_available()
	{
		return $this->available;
	}


	public function is_dir($dir ) {
		// get current directory
		$original_directory = ftp_pwd( $this->conn );
		// test if you can change directory to $dir
		// suppress errors in case $dir is not a file or not a directory
		if ( @ftp_chdir( $this->conn, $dir ) ) {
			// If it is a directory, then change the directory back to the original directory
			ftp_chdir( $this->conn, $original_directory );
			return true;
		}
		else {
			return false;
		}
	}

	public function upload($ftp_m3u_dir, $m3u_dir, $filename)
	{
		if (!$this->is_dir($ftp_m3u_dir)) {
			if (!@ftp_mkdir($this->conn, $ftp_m3u_dir)) {
				return false;
			}
		}

		if (!@ftp_put($this->conn, $ftp_m3u_dir.'/'.$filename, $m3u_dir.'/'.$filename, FTP_BINARY))
			return false;

		return true;
	}

	public function delete($ftp_m3u_dir, $filename)
	{
		if (!@ftp_delete($this->conn, $ftp_m3u_dir.'/'.$filename))
			return false;

		return true;
	}


}

?>
