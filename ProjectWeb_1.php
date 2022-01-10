<!DOCTYPE html>
<html>
<head>
	<title>ProjectWeb</title>
	<style type="text/css">
		body {
			width: 100%;
			height: 100%;
			padding-top: 9%;
		}
		label
		{
			width: 350px;
			height: 20px;
			font-weight: bold;
		}
		input {
			width: 350px;
			height: 20px;
			border-radius: 0px;
			border: 1px solid black;
			background-color: rgba(255,255,0,0.5);
		}
		div	{
			padding: 10px;
			width: 500px;
			border-radius: 20px;
			margin: 25px;
			align-items: center;
		}
		#submit {
			width: 100px;
			height: 25px;
			background-color: rgba(0,255,0,0.5);
			border-radius: 20px;
		}
	</style>
</head>
<body>
	<center>
	<form method="POST" action="ProjectWeb_1.php">
		<label>Enter the source icao code</label><br>
		<input type="text" name="src" id="src"><br><br>
		<label>Enter the destination icao code</label><br>
		<input type="text" name="dst" id="dst"><br><br>
		<input type="submit" id="submit" name="Submit">
	</form>
	</center>
<?php
	if($_POST['src'] && $_POST['dst'])
	{
	$Name = "./Run.exe"." ".$_POST['src']." ".$_POST['dst'];
	$result = shell_exec($Name);
	echo "<center><div>";
	echo $result."</br>";
	echo "</div></center>";
	}
?>
</body>
</html>
