<?php

	function __multiply($m, $n, $out, $mheight, $mwidth, $nwidth)
	{
		echo "{\r\n";
		#define MATRIX_LOOP(w,h)               for (unsigned x = 0; x < (w); x++) for (unsigned y = 0; y < (h); y++)
		
        for ($x = 0; $x < $nwidth; $x++)
			for ($y = 0; $y < $mheight, $y++)
			{
				echo $out."[".($x*$mheight+$y)."] = $m[$y]*$n[".($x*$mwidth)."]";
				for ($k = 1; $k < $mwidth; $k++)
					echo "+$m[".($k*$mheight+$y)."]*$n[".($x*$mwidth+$k)."]";
				echo ";\r\n";
			}
        }
		
		echo "}\r\n";
	}


?>