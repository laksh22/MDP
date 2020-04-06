package ntu.mdpg1app.model;

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
	public static String binToHex(String s) {

		String obstacles = "";
		String binaryStr = s;
		for(int i = 0; i < binaryStr.length(); i+=4){
			int decimal = Integer.parseInt(binaryStr.substring(i,i+4),2);
			String hexStr = Integer.toString(decimal,16);
			obstacles += hexStr;
		}
		return obstacles;
		//return new BigInteger(s, 2).toString(16);
	}
}
