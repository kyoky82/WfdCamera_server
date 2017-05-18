package com.example.jareld.wfdcamera_server;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;

import java.util.HashMap;
import java.util.List;

/*
 *  @项目名：  TestWifiDerect 
 *  @包名：    com.example.lyc2.testwifiderect.adapter
 *  @文件名:   DeviceAdapter
 *  @创建者:   LYC2
 *  @创建时间:  2016/11/24 11:46
 *  @描述：    TODO
 */
public class DeviceAdapter
        extends RecyclerView.Adapter<DeviceAdapter.MyHolder>
{
    private boolean mIsServer;

    //来一个点击的回调
    public interface ItemButtonClickConnectListener {
        void onItemButtonClickConnectListener(int position);
    }

    private ItemButtonClickConnectListener mItemButtonClickConnectListener;

    public void setOnItemButtonClickConnectListener(ItemButtonClickConnectListener i) {
        this.mItemButtonClickConnectListener = i;
    }


    private List<HashMap<String, String>> mList;

    public DeviceAdapter() {}

    public DeviceAdapter(List<HashMap<String, String>> list, boolean isServer) {
        super();
        this.mList = list;
        this.mIsServer = isServer;
    }

    @Override
    public DeviceAdapter.MyHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        LayoutInflater inflater = LayoutInflater.from(parent.getContext());

        View     view     = inflater.inflate(R.layout.recycle_item, parent, false);
        MyHolder myHolder = new MyHolder(view);
        return myHolder;
    }

    @Override
    public void onBindViewHolder(DeviceAdapter.MyHolder holder, final int position) {
        holder.mTv_name.setText("设备名称：" + mList.get(position)
                                               .get("name"));
        holder.mTv_address.setText("设备地址：" + mList.get(position)
                                                  .get("address"));

        if (mItemButtonClickConnectListener != null) {
            holder.mBtn_connect.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    if (!mIsServer) {
                        mItemButtonClickConnectListener.onItemButtonClickConnectListener(position);
                    }

                }
            });
        }

    }

    @Override
    public int getItemCount() {
        return mList.size();
    }

    public class MyHolder
            extends RecyclerView.ViewHolder
    {
        public TextView mTv_name;
        public TextView mTv_address;
        public Button   mBtn_connect;

        public MyHolder(View itemView) {
            super(itemView);
            mTv_name = (TextView) itemView.findViewById(R.id.tv_name);
            mTv_address = (TextView) itemView.findViewById(R.id.tv_address);
            mBtn_connect = (Button) itemView.findViewById(R.id.item_connect);
            if (mIsServer) {
                //如果是服务器 那么就等待连接   不能被点击
                mBtn_connect.setText("服务器端搜索到的设备");


            }

        }
    }


}
