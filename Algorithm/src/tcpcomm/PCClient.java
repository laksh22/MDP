package tcpcomm;

import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Scanner;

import datatypes.Message;

public class PCClient {
	
	public static final String RPI_IP_ADDRESS = "192.168.101.1";
	public static final int RPI_PORT = 4042;
	
	private static PCClient _instance;
	private Socket _clientSocket;
	private PrintWriter _toRPi;
	private Scanner _fromRPi;
	
	private PCClient() {
		
	}
	
	public static PCClient getInstance() {
        if (_instance == null) {
            _instance = new PCClient();
        }
        return _instance;
    }
	
	public static void main (String[] args) throws UnknownHostException, IOException {
		
		PCClient pcClient = PCClient.getInstance();
		pcClient.setUpConnection(RPI_IP_ADDRESS, RPI_PORT);
		System.out.println("RPi successfully connected");
		while (true) {
			pcClient.sendMessage(Message.READ_SENSOR_VALUES);
			String msgReceived = pcClient.readMessage();
			System.out.println("Message received: "+ msgReceived);
		}	
	}
	
	public void setUpConnection (String IPAddress, int portNumber) throws UnknownHostException, IOException{
		_clientSocket = new Socket(RPI_IP_ADDRESS, RPI_PORT);
		_toRPi = new PrintWriter(_clientSocket.getOutputStream());
		_fromRPi = new Scanner(_clientSocket.getInputStream());
	}
	
	public void closeConnection() throws IOException {
		if (!_clientSocket.isClosed()) {
			_clientSocket.close();
		}
	}

	public void sendMessage(String msg) throws IOException {
		
		_toRPi.print(msg);
		_toRPi.flush();
		
		System.out.println("Message sent: " + msg);
	}

	public String readMessage() throws IOException {

		String messageReceived = _fromRPi.nextLine();
		System.out.println("Message received: " + messageReceived);
		
		return messageReceived;
	}
         
}
