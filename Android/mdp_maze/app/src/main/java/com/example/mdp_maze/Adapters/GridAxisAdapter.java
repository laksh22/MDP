package com.example.mdp_maze.Adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.TextView;

import com.example.mdp_maze.R;

public class GridAxisAdapter extends BaseAdapter {

    private Context mContext;
    private int[] mItems;

    public GridAxisAdapter(Context context, int numCoordinates){
        mContext = context;
        mItems = new int[numCoordinates];

        for(int i=0; i < numCoordinates; ++i)
            mItems[i] = i;
    }

    @Override
    public int getCount() {
        return mItems.length;
    }

    @Override
    public Object getItem(int index) {
        return mItems[index];
    }

    @Override
    public long getItemId(int i) {
        return 0;
    }

    @Override
    public View getView(int index, View view, ViewGroup viewGroup) {
        if(view == null){
            view = LayoutInflater.from(mContext).inflate(R.layout.cell_layout,viewGroup, false);
        }
        TextView numTextView = (TextView) view.findViewById(R.id.tv_cell);
        numTextView.setText(Integer.toString(mItems[index]));
        view.setBackgroundResource(R.drawable.cell_item_axis);
        return view;

    }
}
