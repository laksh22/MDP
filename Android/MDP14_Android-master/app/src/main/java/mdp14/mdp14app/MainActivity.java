package mdp14.mdp14app;

import android.app.Dialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentTransaction;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.GridLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import com.example.android.common.logger.Log;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;

import mdp14.mdp14app.bluetooth.BluetoothChatFragment;
import mdp14.mdp14app.model.HexBin;
import mdp14.mdp14app.model.IDblock;
import mdp14.mdp14app.model.Map;
import mdp14.mdp14app.model.Position;
import mdp14.mdp14app.model.Robot;
import mdp14.mdp14app.model.WayPoint;

public class MainActivity extends AppCompatActivity {
    GridLayout base_layout;
    BluetoothChatFragment fragment;
    MenuItem menu_set_robot_position;
    MenuItem menu_set_waypoint;
    MenuItem menu_remove_waypoint;

    MenuItem menu_update_map;
    MenuItem menu_auto_update_map;
    MenuItem menu_set_config1;
    MenuItem menu_set_config2;
    MenuItem menu_enable_swipe_input;
    MenuItem menu_show_bluetooth_chat;
    MenuItem menu_show_data_strings;


    TextView tv_status;
    Button btn_forward;
    Button btn_left;
    Button btn_right;
    Button btn_terminate;
    Button btn_explr;
    Button btn_fastest;
    Button btn_config1;
    Button btn_config2;

    String status = "Idle";

    //For robot fastest-path animation
    String[] originalMovement = new String[0]; //if there are multiple movements
    ArrayList<String> simplifiedMovement = new ArrayList<String>();

    //





    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //initialize all the buttons
        tv_status = (TextView) findViewById(R.id.tv_status);
        btn_forward = (Button) findViewById(R.id.btn_forward);
        btn_left= (Button) findViewById(R.id.btn_left);
        btn_right= (Button) findViewById(R.id.btn_right);
        btn_terminate= (Button) findViewById(R.id.btn_terminate);
        btn_explr= (Button) findViewById(R.id.btn_explr);
        btn_fastest= (Button) findViewById(R.id.btn_fastest);
        btn_config1= (Button) findViewById(R.id.btn_config1);
        btn_config2= (Button) findViewById(R.id.btn_config2);
        base_layout = (GridLayout) findViewById(R.id.base_layout);

        //initialize the bluetooth service component
        if (savedInstanceState == null) {
            FragmentTransaction transaction = getSupportFragmentManager().beginTransaction();
            fragment = new BluetoothChatFragment();
            transaction.replace(R.id.sample_content_fragment, fragment);
            transaction.commit();
        }

        //initialize default status "idle"
        updateStatus(status);

        //initialize listener for all the buttons
        setBtnListener();

        //initialize the grid
        loadGrid();
    }

    //method to update the label textview
    public void updateStatus(String status){
        this.status = status;
        tv_status.setText(status);
    }

    //method to set all the event for buttons
    private void setBtnListener(){
        btn_forward.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                //check if robot is out of bounds
                if(!Robot.getInstance().isOutOfBounds()) {
                    Robot.getInstance().moveForward(10);
                    outgoingMessage("W");
                    loadGrid();
                }
            }
        });
        btn_left.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Robot.getInstance().rotateLeft();
                outgoingMessage("A");
                loadGrid();
            }
        });
        btn_right.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                Robot.getInstance().rotateRight();
                outgoingMessage("D");
                loadGrid();
            }
        });
        btn_terminate.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                outgoingMessage(STATUS_TERMINATE_HEADER);
                updateStatus(STATUS_TERMINATE_DESC);
            }
        });
        btn_explr.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                String positionX = String.valueOf(Robot.getInstance().getPosX()) ;
                String positionY = String.valueOf(Robot.getInstance().getPosY()) ;
                String direction = String.valueOf(Robot.getInstance().getDirection());
                String exploringMsg;
//                if (WayPoint.wp.getPosition() == null) {
//                    /*/status = "Setting WayPoint";
//                    updateStatus(status);
//
//                    menu_set_robot_position.setChecked(false);
//                    menu_set_waypoint.setChecked(true);
//                    Toast toast=Toast.makeText(getApplicationContext(),"Tap the Grid to set WayPoint",Toast.LENGTH_LONG);
//                    toast.show();/*/
//                    exploringMsg = STATUS_EX_HEADER.concat("|").concat(positionX).concat(",").concat(positionY).concat(",").concat(direction).concat("||");
//                }else{
//                    String wpX = String.valueOf(WayPoint.getInstance().getPosition().getPosX());
//                    String wpY = String.valueOf(WayPoint.getInstance().getPosition().getPosY());
//                    exploringMsg = STATUS_EX_HEADER.concat("|").concat(positionX).concat(",").concat(positionY).concat(",").concat(direction).concat("||").concat(wpX).concat(",").concat(wpY);
//
//                }
                outgoingMessage("BEGINEX");
                updateStatus(STATUS_EX_DESC);
            }
        });
        btn_fastest.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                if (WayPoint.wp.getPosition() == null) {
                    status = "Setting WayPoint";
                    updateStatus(status);

                    menu_set_robot_position.setChecked(false);
                    menu_set_waypoint.setChecked(true);
                    Toast toast=Toast.makeText(getApplicationContext(),"Tap the Grid to set WayPoint",Toast.LENGTH_LONG);
                    toast.show();

                }else{
                    String positionX = String.valueOf(Robot.getInstance().getPosX()) ;
                    String positionY = String.valueOf(Robot.getInstance().getPosY()) ;
                    String direction = String.valueOf(Robot.getInstance().getDirection());
                    String wpX = String.valueOf(WayPoint.getInstance().getPosition().getPosX());
                    String wpY = String.valueOf(WayPoint.getInstance().getPosition().getPosY());
                    String FPMsg = STATUS_FP_HEADER.concat("|").concat(positionX).concat(",").concat(positionY).concat(",").concat(direction).concat("||").concat(wpX).concat(",").concat(wpY);
                    outgoingMessage("BEGINFP");
                    updateStatus(STATUS_FP_DESC);
                }


            }
        });
        btn_config1.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
                String retrievedText = prefs.getString("string1", null);
                if (retrievedText != null) {
                    outgoingMessage(retrievedText);
                }
            }
        });
        btn_config2.setOnClickListener(new View.OnClickListener() {
            public void onClick(View v) {
                SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
                String retrievedText = prefs.getString("string2", null);
                if (retrievedText != null) {
                    outgoingMessage(retrievedText);
                }
            }
        });
    }

    //initalize all the menu item button
    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        super.onPrepareOptionsMenu(menu);
        menu_set_robot_position = menu.findItem(R.id.action_set_robot_position);
        menu_set_waypoint = menu.findItem(R.id.action_set_waypoint);
        menu_remove_waypoint = menu.findItem(R.id.action_remove_waypoint);
        menu_update_map = menu.findItem(R.id.action_update_map);
        menu_auto_update_map = menu.findItem(R.id.action_auto_update_map);
        menu_set_config1 = menu.findItem(R.id.action_set_config_string1);
        menu_set_config2 = menu.findItem(R.id.action_set_config_string2);
        menu_show_data_strings = menu.findItem(R.id.action_view_data_strings);
        menu_enable_swipe_input = menu.findItem(R.id.action_enable_swipe_input);
        menu_show_bluetooth_chat = menu.findItem(R.id.action_show_bluetooth_chat);
        return true;
    }

    //load/reload the Grid View into the page
    private void loadGrid(){
        MapCanvas mCustomDrawableView = new MapCanvas(this);
        //set touch event and swipe event
        if(menu_enable_swipe_input!=null&&menu_enable_swipe_input.isChecked()){
            mCustomDrawableView.setGesture(this);
        }else{
            mCustomDrawableView.setOnTouchListener(mCustomDrawableView);
        }
        base_layout.removeAllViews();
        base_layout.addView(mCustomDrawableView);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    //this is the event when menu item is clicked
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        //noinspection SimplifiableIfStatement

        if (id == R.id.action_set_robot_position) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            return true;
        }
        if (id == R.id.action_set_waypoint) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            return true;
        }
        if (id == R.id.action_remove_waypoint) {
            removeWaypoint();
            return true;
        }
        if (id == R.id.action_update_map) {
            loadGrid();
            return true;
        }
        if (id == R.id.action_auto_update_map) {
            boolean checked = item.isChecked();
            item.setChecked(!checked);
            if(item.isChecked()){
                loadGrid();
            }
            return true;
        }
        if (id == R.id.action_set_config_string1) {
            setConfiguredString(1);
            return true;
        }
        if (id == R.id.action_set_config_string2) {
            setConfiguredString(2);
            return true;
        }

        if (id == R.id.action_view_data_strings) {
            displayDataStrings();
            return true;
        }

        if (id == R.id.action_enable_swipe_input) {
            boolean checked = item.isChecked();
            clearAllEditableCheckbox();
            item.setChecked(!checked);
            loadGrid();
            return true;
        }

        if (id == R.id.action_show_bluetooth_chat) {
            boolean checked = item.isChecked();
            item.setChecked(!checked);
            if(fragment!=null){
                fragment.showChat(!checked);
            }
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    //this method opens up a dialog to save input string on the phone
    //index is the key of the string.
    //eg string 1 is stored in 'string1', string 2 in 'string2'
    private void setConfiguredString(final int index){
        final EditText txtField = new EditText(this);
        SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
        String retrievedText = prefs.getString("string"+index, null);
        if (retrievedText != null) {
            txtField.setText(retrievedText);
        }

        new AlertDialog.Builder(this)
                .setTitle("Configure String "+index)
                .setView(txtField)
                .setPositiveButton("Save", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                        String input = txtField.getText().toString();
                        SharedPreferences.Editor editor = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE).edit();
                        editor.putString("string"+index, input);
                        editor.apply();
                    }
                })
                .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int whichButton) {
                    }
                })
                .show();
    }

    private void displayDataStrings(){
        //final EditText txtField = new EditText(this);
        //SharedPreferences prefs = getSharedPreferences(String.valueOf(R.string.app_name), MODE_PRIVATE);
        //String retrievedText = prefs.getString("string"+index, null);
        //if (retrievedText != null) {
        //    txtField.setText(retrievedText);
        //}

        // custom dialog


        // create an alert builder
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("MDF + Image String");

        // set the custom layout
        final View customLayout = getLayoutInflater().inflate(R.layout.data_strings, null);
        builder.setView(customLayout);

        // add a button
        builder.setPositiveButton("CLOSE", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                // send data from the AlertDialog to the Activity
                dialog.dismiss();
            }
        });

        // set the custom dialog components - text, image and button
        EditText text_mdf1 = (EditText) customLayout.findViewById(R.id.textbox_mdf1);
        String explored = HexBin.binToHex(Map.getInstance().getBinaryExplored());
        text_mdf1.setText(explored);

        EditText text_mdf2 = (EditText) customLayout.findViewById(R.id.textbox_mdf2);
        String obstacles = HexBin.binToHex(Map.getInstance().getBinaryExploredObstacle());
        text_mdf2.setText(obstacles);

        EditText text_imgreg = (EditText) customLayout.findViewById(R.id.textbox_imgreg);
        String imgreg = "{";
        ArrayList<IDblock> numberedBlocks = Map.getInstance().getNumberedBlocks();
        for(IDblock blk : numberedBlocks){
            imgreg += String.format("(%s, %d, %d)", blk.getID(), blk.getPosition().getPosX(), blk.getPosition().getPosY());
            imgreg += ", ";
        }
        imgreg = imgreg.substring(0, imgreg.length()-2);
        imgreg += "}";
        text_imgreg.setText(imgreg);

        // create and show the alert dialog
        AlertDialog dialog = builder.create();
        dialog.show();
    }




    //this is to uncheck all the checkable menuitem
    private void clearAllEditableCheckbox(){
        menu_set_robot_position.setChecked(false);
        menu_set_waypoint.setChecked(false);
        menu_enable_swipe_input.setChecked(false);
    }

    public static final String STATUS_EX_DESC = "Moving (Exploring)";
    public static final String STATUS_EX_HEADER = "EX";
    public static final String STATUS_FP_DESC = "Moving (Fastest Path)";
    public static final String STATUS_FP_HEADER = "FP";
    public static final String STATUS_DONE_DESC = "Stopped";
    public static final String STATUS_DONE_HEADER = "DONE";
    public static final String STATUS_TERMINATE_HEADER = "TE|||"; //change this
    public static final String STATUS_TERMINATE_DESC = "Terminated";

    //method is ran when new message comes in
    public void incomingMessage(String readMsg) {
        //outgoingMessage(readMsg);
        //update map

        final Robot r = Robot.getInstance();

        if(readMsg.length()>0){
            menu_show_bluetooth_chat.setChecked(true);
            fragment.showChat(true);
            final String delimiterPattern = ":";
            String message []= readMsg.split(delimiterPattern);
            if(message[0].equals("GRID")){ //receive mapDescriptor from Algo

                Map.getInstance().setMapJson(message[1]);
                if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()){
                    loadGrid();
                }
            }

            if(message[0].equals("DATA")){
                String data[] = message[1].split(",");

                Map.getInstance().setMap(data[0],"", data[1]);

                r.setPosX(Float.parseFloat(data[2]));
                r.setPosY(Float.parseFloat(data[3]));
                r.setDirection(data[4]);

                if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()){
                    loadGrid();
                }

            }

            //receive numbered block
            if(message[0].equals("BLOCK")) {
                String posAndDirect[] = message[1].split(",");
                IDblock input = new IDblock(posAndDirect[2], Integer.parseInt(posAndDirect[0]), Integer.parseInt(posAndDirect[1]));
                Map.getInstance().addNumberedBlocks(input);
                if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()){
                    loadGrid();
                }
            }

                //receive robot position
            if(message[0].equals("ROBOTPOSITION")){
                String posAndDirect[] = message[1].split(",");
                r.setPosX(Float.parseFloat(posAndDirect[0]));
                r.setPosY(Float.parseFloat(posAndDirect[1]));
                r.setDirection(posAndDirect[2]);

                if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()){
                    loadGrid();
                }

                // updateStatus(STATUS_FP_DESC);


                /*/New animation 2
                movement = message[4].split(","); //if there are multiple movements
                final Handler OverallHandler = new Handler();
                for (int i = 0; i < movement.length; i++){
                    final int finalI = i;

                    Runnable OverAll = new Runnable() {
                        @Override
                        public void run() {
                            if (movement[finalI].contains("F")) {
                                String step[] = movement[finalI].split("F");
                                int noOfSteps = (Integer.parseInt(step[1])/10);

                                Handler ForwardHandler = new Handler();
                                for (int forward=0; forward < noOfSteps; forward++){
                                    Runnable MoveForward = new Runnable() {
                                        @Override
                                        public void run() {
                                            Robot.getInstance().moveForward(10);
                                            if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()) {
                                                loadGrid();
                                            }
                                        }
                                    };
                                    ForwardHandler.postDelayed(MoveForward, (forward + 1) * 150);
                                }
                            }

                            else if (movement[finalI].contains("L")){
                                String step[] = movement[finalI].split("L");
                                int noOfRotation = (Integer.parseInt(step[1])/90);
                                Handler LeftHandler = new Handler();
                                for (int rotation=0; rotation < noOfRotation; rotation++){
                                    Runnable RotateLeft = new Runnable() {
                                        @Override
                                        public void run() {
                                            Robot.getInstance().rotateLeft();
                                            if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()) {
                                                loadGrid();
                                            }
                                        }
                                    };
                                    LeftHandler.postDelayed(RotateLeft, (rotation + 1) * 500);
                                }
                            }

                            //else if (movement[i].contains("R")){
                            else{
                                String step[] = movement[finalI].split("R");
                                int noOfRotation = (Integer.parseInt(step[1])/90);
                                Handler RightHandler = new Handler();
                                for (int rotation=0; rotation < noOfRotation; rotation++){
                                    Runnable RotateRight = new Runnable() {
                                        @Override
                                        public void run() {
                                            Robot.getInstance().rotateRight();
                                            if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()) {
                                                loadGrid();
                                            }
                                        }
                                    };
                                    RightHandler.postDelayed(RotateRight, (rotation + 1) * 500);
                                }
                            }
                        }
                    };

                    OverallHandler.postDelayed(OverAll, (finalI)  * 2700);
                }/*/


                



            }


            if(message[0].equals(STATUS_DONE_HEADER)){ //done
                updateStatus(STATUS_DONE_DESC);
            }

            if(message[1].equals("F")){
                updateStatus("Moving Forward");
            }
            if(message[1].equals("TR")){
                updateStatus("Turning Right");
            }
            if(message[1].equals("TL")){
                updateStatus("Turning Left");
            }
            if(message[1].equals("FP")){
                updateStatus("Fastest Path");
            }
            if(message[1].equals("EX")){
                updateStatus("Exploration");
            }
            if(message[1].equals("DONE")){
                updateStatus("Done!");
            }




        }
        //setMap is for the actual; setMapJson is for AMD

        /*
        JSONObject obj = null;
        try {
            obj = new JSONObject(readMsg);
            if(obj.has("grid")){
                Map.getInstance().setMapJson(obj.getString("grid"));
            }

            if(obj.has("robotPosition")){
                JSONArray arr = obj.getJSONArray("robotPosition");
                double x = arr.getDouble(0);
                double y = arr.getDouble(1);
                double dir = arr.getDouble(2);
                Robot r = Robot.getInstance();
                r.setDirection((float) dir);
                r.setPosX((float) x+1);
                r.setPosY(19-(float) y-1);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        */

        /*if(menu_auto_update_map!=null&&menu_auto_update_map.isChecked()){
            loadGrid();
        }*/

    }

    /*/public class RunQueue implements Runnable{
        private List list = new ArrayList();

        public void queue(Runnable task)
        {
            list.add(task);
        }

        public void run()
        {
            while(list.size() > 0)
            {
                Runnable task = (Runnable) list.get(0);

                list.remove(0);
                task.run();
            }
        }
    }/*/

    //method to send out message to rpi thru bluetooth
    public boolean outgoingMessage(String sendMsg) {
        return fragment.sendMsg("@t" + sendMsg + "!");
    }
    public boolean outgoingMessage(String sendMsg, int destination) {
        //add delimiters
        // 0=algo, 1=ardu
        if(destination == 0){
            sendMsg = "@t" + sendMsg + "!";
        }else if(destination == 1){
            sendMsg = "@s" + sendMsg + "!";
        }

        return fragment.sendMsg(sendMsg);
    }
    //on the coordinate tapped
    //set waypoint, if menuitem is checked
    //set robot, if menuitem is checked
    public void onGridTapped(int posX, int posY) {
        if(menu_set_robot_position.isChecked()){
            Robot r = Robot.getInstance();
            r.setPosX(posX);
            r.setPosY(posY);
            r.setDirection("NORTH");
            outgoingMessage("START:"+(int)posX+","+(int)posY);

            //Make a prompt here to confirm
            menu_set_robot_position.setChecked(false);
        }
        if(menu_set_waypoint.isChecked()){
            Position p = new Position(posX,posY);
            WayPoint.getInstance().setPosition(p);
            outgoingMessage("WP:"+(int)posX+","+(int)posY);

            //Make a prompt here to confirm
            menu_set_waypoint.setChecked(false);
        }
        loadGrid();
    }

    //clear waypoint
    private void removeWaypoint(){
        WayPoint.getInstance().setPosition(null);
        loadGrid();
    }

    //swipe gesture input
    public void onSwipeTop() {

        //check if robot is out of bounds
        if(!Robot.getInstance().rotateToNorth()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("MOVE:F");
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToNorth();
        }
        loadGrid();
    }

    public void onSwipeLeft() {
        if(!Robot.getInstance().rotateToWest()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("MOVE:F");
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToWest();
        }

        loadGrid();
    }

    public void onSwipeRight() {
        if(!Robot.getInstance().rotateToEast()){
            if(!Robot.getInstance().isOutOfBounds()) {
                outgoingMessage("MOVE:F");
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToEast();
        }
        loadGrid();
    }

    public void onSwipeBottom() {
        if(!Robot.getInstance().rotateToSouth()){
            if(!Robot.getInstance().isOutOfBounds()) {

                outgoingMessage("MOVE:F");
                //getack
                Robot.getInstance().moveForward(10);
            }
        }
        else{
            Integer count = Robot.getInstance().getCount();
            sendMsgOnSwipe(count);
            //getack
            Robot.getInstance().rotateRobotToSouth();
        }
        loadGrid();
    }

    public void sendMsgOnSwipe(Integer count){

        if(count==1){
            outgoingMessage("MOVE:TR");
        }
        if(count==2){
            outgoingMessage("MOVE:TR");
            outgoingMessage("MOVE:TR");
        }
        if(count==-1){
            outgoingMessage("MOVE:TL");
        }

    }

    @Override
    protected void onStop() {
        // call the superclass method first
        super.onStop();

    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }

}

