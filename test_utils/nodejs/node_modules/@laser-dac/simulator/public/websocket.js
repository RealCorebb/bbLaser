function WebSocketClient() {
  this.autoReconnectInterval = 1 * 1000; // ms
}
WebSocketClient.prototype.open = function (url) {
  this.url = url;
  this.instance = new WebSocket(this.url);
  this.instance.onopen = this.onopen;
  this.instance.onmessage = (event) => {
    this.onmessage(event);
  };
  this.instance.onclose = (e) => {
    switch (e.code) {
      case 1000: // CLOSE_NORMAL
        console.log('WebSocket: closed');
        break;
      default:
        // Abnormal closure
        this.reconnect(e);
        break;
    }
    this.onclose(e);
  };
  this.instance.onerror = (e) => {
    switch (e.code) {
      case 'ECONNREFUSED':
        this.reconnect(e);
        break;
      default:
        this.onerror(e);
        break;
    }
  };
};
WebSocketClient.prototype.send = function (data, option) {
  try {
    this.instance.send(data, option);
  } catch (e) {
    this.instance.emit('error', e);
  }
};
WebSocketClient.prototype.reconnect = function (e) {
  console.log(`WebSocketClient: retry in ${this.autoReconnectInterval}ms`, e);
  var that = this;
  setTimeout(function () {
    console.log('WebSocketClient: reconnecting...');
    that.open(that.url);
  }, this.autoReconnectInterval);
};
WebSocketClient.prototype.onopen = function (e) {
  console.log('WebSocketClient: open', arguments);
};
WebSocketClient.prototype.onmessage = function (e) {
  console.log('WebSocketClient: message', arguments);
};
WebSocketClient.prototype.onerror = function (e) {
  console.log('WebSocketClient: error', arguments);
};
WebSocketClient.prototype.onclose = function (e) {
  console.log('WebSocketClient: closed', arguments);
};

export default WebSocketClient;
