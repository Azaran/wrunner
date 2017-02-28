<?php

// Initialization part ////////////////////////////////////////////////////

$servername = "localhost";
$username = "";
$password = "";
$dbname = "icdf2c";

$project = "icdf2c";
$path_to_project = "/home/boincadm/projects/$project"; 
$path_to_app = $path_to_project . "/apps/$project/";
$permissions = 0755;

$runner = "wrunner";

$path_to_release = "/home/boincadm/release/";
$path_to_previous = "";
$last_cracker_version = "";
$last_runner_version = "";

$platform = "/x86_64-pc-linux-gnu/";

$release_name = [ // [ "name of binary file" => [ "release file name", "last version" ]
    "fitcrack" => [ "fitcrack-cpu", 0 ], 
    "wrunner" => [ "wrunner", 0 ]
];

$support_files = [
    "lower_latin.txt",
    "upper_latin.txt",
    "kernels55.zip"
];

///////////////////////////////////////////////////////////////////////////
// START OF THE SCRIPT ////////////////////////////////////////////////////

$username = $argv[1];
$password = $argv[2];

$next_id = getNextId();

// Make new directory
$new_dir_name = $path_to_app . $next_id . $platform;


$file_names = [ ];
foreach ($release_name as $binary => $relase)
    array_push($file_names, [ $new_dir_name , $binary , $next_id ]; 


// Create files
if (checkNewRelease($file_names)) {

    mkNewDir($new_dir_name);
   
    foreach ($support_files as $file) 
       array_push($file_names, [ $new_dir_name , $file, "" ]);
    // print_r($file_names);

    if (copyFiles($file_names)) {
	updateBoinc();
    } else
	exit(-1);
} else
      echo "There is no new release version.\n";

// END OF THE SCRIPT //////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
function updateBoinc() {
   
   global $path_to_project;
   system($path_to_project . "/update_versions");
};

function writeXML($directory) {
    
    global $runner;

    $dir_content = array_diff(scandir($directory), array('.', '..'));
    $msg =  "<version>\n";

    foreach ($dir_content as $file) {
	if (strpos($file, $runner) !== false) {
	    $msg .= "	<file><physical_name>$file</physical_name>\n";
	    $msg .= "	    <copy_file/>\n";
	    $msg .= "	    <main_program/>\n";
	    $msg .= "	</file>\n";
	} else
	    $msg .= "	<file><physical_name>$file</physical_name><copy_file/></file>\n";
    }

    $msg .=  "</version>\n";

    echo "version.xml:\n $msg";
    
    $xml = fopen($directory . "version.xml", "w") or die ("Couldn't open file version.xml");
    fwrite($xml, $msg);
    fclose($xml);
};

function copyFiles($file_names){

    global $release_name, $path_to_release, $path_to_previous;

    // echo "Files: \n";

    foreach ($file_names as $file) {
	if (array_key_exists($file[1], $release_name)) {
	    if (newRelease([ $path_to_previous , $file[1] ], [ $path_to_release, $file[1] ]) ) {

		echo "Copy " . $file[1] . " from release. " . " \n";
		$source = $path_to_release . $release_name[$file[1]][0];
		echo "Source " . $source . "\n";
		$dest = $file[0]. $file[1]. ($release_name[$file[1]][1]+1);
		echo "Dest   " . $dest . "\n";

	    } else {

		echo "Copy " . $file[1] . " from ". $path_to_previous . " .\n"; 
		$source = $path_to_previous . $file[1] . $release_name[$file[1]][1] ;
		echo "Source " . $source . "\n";
		$dest = $file[0]. $file[1]. $release_name[$file[1]][1];
		echo "Dest   " . $dest . "\n";
	    }
	} else {

	    $source = $path_to_previous . $file[1];
	    echo "Source " . $source . "\n";
	    $dest = $file[0] . $file[1];
	    echo "Dest   " . $dest . "\n";
	}

	if (!copy($source , $dest)) {

	    echo ("Couldn't copy the file $source to $dest !\n");
	    return false;
	}
	echo "Successfully copied $source to $dest.\n";
    }

    writeXML($file[0]);

    return true;
};

function parseFilePath($file_path) {

    global $release_name;

    // echo "Filename: $file_path\n";
    Preg_match('#^(.*)\/(.*)\/.*\/([a-zA-Z]+)([0-9]+)$#', $file_path, $matches);

    //	print_r($matches);

    $ret['path'] = $matches['1'];
    $ret['version_dir'] = $matches['2'];
    $ret['bin_name'] = $matches['3'];
    $ret['bin_version'] = $matches['4'];

    $release_name[$ret['bin_name']][1] = $ret['bin_version'];

    return $ret;
};

function newRelease($file1, $file2) {

    $last_version_modified = modificationTime($file1[0], $file1[1]);	
    $release_modified = modificationTime($file2[0], $file2[1]);
    
    return $release_modified < $last_version_modified ? false : true;
}

function checkNewRelease($file_paths) {

    global $path_to_release, $path_to_previous;

    $i = 0;
    foreach ($file_paths as $file_path) {

	$parsed_path = parseFilePath($file_path[0] . $file_path[1] . $file_path[2]);

	findPrevVersionDir($parsed_path['path'], $parsed_path['version_dir']);

	if (newRelease([ $path_to_previous, $parsed_path['bin_name'] ], [$path_to_release, $parsed_path['bin_name'] ]))
	    $i++; 
    }

    getBinaryFileVersion();

    return ($i > 0 ? true : false);
};

function findPrevVersionDir($path, $version_dir) {

    global $platform, $path_to_previous;
// Find nonempty folder with previous version
    do {
	while (!file_exists($path . "/" . $version_dir . "/")) 
	    $version_dir--;

	$path_to_previous = $path . "/" . $version_dir . $platform;
	$dir_content = array_diff(scandir($path_to_previous), array('.', '..'));

	//	    print_r($dir_content);

    } while (empty($dir_content) && $version_dir-- > 0);


    // echo "Path to prev: " . $path_to_previous . "\n";
};

function getBinaryFileVersion() {

    global $release_name, $path_to_previous;

    $dir_content = array_diff(scandir($path_to_previous), array('.', '..'));

    foreach ($dir_content as $file) {
	foreach ($release_name as $key => $name){

//	    echo "key $key => file $file\n";
	    if (strpos($file, $key) !== false) {
		preg_match('#^'.$key.'([0-9]+)$#', $file, $matches);
		$release_name[$key][1] = $matches[1];
		// echo "key: $key => v: " . $release_name[$key][1] . "\n";
	    }
	}
    }
};

function modificationTime($path_to_file, $file_name) {

    global $release_name, $path_to_release;

    $dir_content = array_diff(scandir($path_to_file), array('.', '..'));
    $modified = 0;

    if ($path_to_file == $path_to_release)
	$file_name = $release_name[$file_name][0];

    foreach ($dir_content as $file)
	if (strpos($file, $file_name) !== false)
	    echo "$file was last modified: " . date ("F d Y H:i:s.", $modified = filemtime($path_to_file . $file)) . "\n";

    return $modified;
};

function getNextId() {

    global $servername, $username, $password, $dbname;

    // Create connection
    $conn = new mysqli($servername, $username, $password, $dbname);

    if ($conn->connect_error)
	die("Connection failed: " . $conn->connect_error);

    $sql = "SHOW TABLE STATUS LIKE 'app_version'";
    $result = $conn->query($sql);

    $next_id = 0;

    $status = $result->fetch_assoc();
    $next_id = $status['Auto_increment'];

    // Close connection
    $conn->close();

    return $next_id;
};

function mkNewDir($new_dir_name) {

    global $permissions;

    echo "Creating directory $new_dir_name with permissions $permissions.\n";

    if (file_exists($new_dir_name))
	die ("Directory $new_dir_name already exists.\n");

    $old = umask(0);
    if (!mkdir($new_dir_name, $permissions, true)) {
	umask($old);
	die ("You don't have permissions to create folder $new_dir_name.\n");
    }

    umask($old);

    echo "Folder was succesfully created.\n";
};

?>
