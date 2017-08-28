<?php
/*
 * Bacula(R) - The Network Backup Solution
 * Baculum   - Bacula web interface
 *
 * Copyright (C) 2013-2017 Kern Sibbald
 *
 * The main author of Baculum is Marcin Haba.
 * The original author of Bacula is Kern Sibbald, with contributions
 * from many others, a complete list can be found in the file AUTHORS.
 *
 * You may use this file and others of this release according to the
 * license defined in the LICENSE file, which includes the Affero General
 * Public License, v3.0 ("AGPLv3") and some additional permissions and
 * terms pursuant to its AGPLv3 Section 7.
 *
 * This notice must be preserved when any source code is
 * conveyed and/or propagated.
 *
 * Bacula(R) is a registered trademark of Kern Sibbald.
 */

class FileSetsInfo extends BaculumAPIServer {

	public function get() {
		$filesets = $this->getModule('fileset')->getFileSets();
		$allowed_filesets = $this->getModule('bconsole')->bconsoleCommand($this->director, array('.fileset'), $this->user);
		if ($allowed_filesets->exitcode === 0) {
			if (is_array($filesets) && count($filesets) > 0) {
				$fs = array();
				for ($i = 0; $i < count($filesets); $i++) {
					if(in_array($filesets[$i]->fileset, $allowed_filesets->output)) {
						$fs[] = $filesets[$i];
					}
				}
				$this->output = $fs;
				$this->error = FileSetError::ERROR_NO_ERRORS;
			} else {
				$this->output = FileSetError::MSG_ERROR_FILESET_DOES_NOT_EXISTS;
				$this->error = FileSetError::ERROR_FILESET_DOES_NOT_EXISTS;
			}
		} else {
			$this->output = $allowed_filesets->output;
			$this->error = $allowed_filesets->exitcode;
		}
	}
}

?>