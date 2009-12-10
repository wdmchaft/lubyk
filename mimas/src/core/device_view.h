#ifndef MIMAS_SRC_CORE_DEVICE_VIEW_H_
#define MIMAS_SRC_CORE_DEVICE_VIEW_H_

class DeviceProxy;

class DeviceView : public TreeViewItem, public Observer, public Observable {
public:

  DeviceView(DeviceProxy *proxy);

  DeviceProxy *device_proxy() {
    return device_proxy_;
  }

  virtual void paintItem(Graphics& g, int width, int height);

  bool mightContainSubItems() {
    return true;
  }

  void itemOpennessChanged( bool isNowOpen);

private:
  DeviceProxy *device_proxy_;
  bool subnodes_added_;
};

#endif // MIMAS_SRC_CORE_DEVICE_VIEW_H_