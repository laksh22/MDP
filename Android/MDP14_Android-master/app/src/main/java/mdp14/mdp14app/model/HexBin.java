package mdp14.mdp14app.model;

import java.math.BigInteger;

public class HexBin {
	public static String hexToBin(String s) {
		if(s!=null){
			s="F"+s;
			s = new BigInteger(s, 16).toString(2);
			s = s.substring(4);
		  return s ;
		}
		return "";
	}
	public static String BinTohex(String s) {
		  return new BigInteger(s, 2).toString(16);
	}
}
