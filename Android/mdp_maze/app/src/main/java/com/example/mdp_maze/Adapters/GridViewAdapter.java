package com.example.mdp_maze.Adapters;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.example.mdp_maze.CoordinatesSelectionService;
import com.example.mdp_maze.MainActivity;
import com.example.mdp_maze.R;

import java.util.ArrayList;

public class GridViewAdapter extends BaseAdapter {

    private Context mContext;
    private int mNumRows;
    private int mNumCols;
    private ArrayList<Character> mMapDescriptor;

    private static final char FREE  ='1';
    private static final char OBSTACLE = '2';
    private static final char ROBOT_HEAD = '3';
    private static final char ROBOT_BODY = '4';

    public GridViewAdapter(Context context, int numRows, int numCols, ArrayList<Character> mapDescriptor){
        mContext = context;
        mNumRows = numRows;
        mNumCols = numCols;
        mMapDescriptor = new ArrayList<Character>(mapDescriptor);
    }

    @Override
    public int getCount() {
        return mNumRows * mNumCols;
    }

    @Override
    public Object getItem(int index) {
        return mMapDescriptor.get(index);
    }

    @Override
    public long getItemId(int index) {
        return index;
    }

    @Override
    public View getView(final int index, View currentView, ViewGroup parentViewGroup) {

        if(currentView == null){
            currentView = LayoutInflater.from(mContext).inflate(R.layout.cell_layout,parentViewGroup, false);
        }

        final int xCoord = index % mNumCols;
        final int yCoord = index / mNumCols;

        TextView cellTV = (TextView) currentView.findViewById(R.id.tv_cell);
        cellTV.setText(Integer.toString(xCoord) + ", " + Integer.toString(yCoord));

        char cellStatus = (char) getItem(index);
        setCellColor(currentView, cellStatus);

        currentView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                Intent messageIntent = new Intent(mContext, CoordinatesSelectionService.class);
                messageIntent.putExtra("X", xCoord);
                messageIntent.putExtra("Y", yCoord);
                ((Activity)mContext).startActivityForResult(messageIntent, MainActivity.REQUEST_COORDINATES);
            }
        });

        return currentView;
    }

    private void setCellColor(View view, char cellStatus){

        switch(cellStatus){
            case FREE:
                view.setBackgroundResource(R.drawable.cell_item_1);
                break;
            case OBSTACLE:
                view.setBackgroundResource(R.drawable.cell_item_2);
                break;
            case ROBOT_HEAD:
                view.setBackgroundResource(R.drawable.cell_item_3);
                break;
            case ROBOT_BODY:
                view.setBackgroundResource(R.drawable.cell_item_4);
                break;
            default:
                view.setBackgroundResource(R.drawable.cell_item_0);
        }
    }

    public void refreshMap(ArrayList<Character> mapDescriptor){
        mMapDescriptor.clear();
        mMapDescriptor.addAll(mapDescriptor);
        Log.d("GridViewAdapter", "Map Descriptor Size: " + mapDescriptor.size());
        notifyDataSetChanged();
    }

}

