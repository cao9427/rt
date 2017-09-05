pragma Singleton
import QtQuick 2.0
import Hyby.RT 1.0


Item {
    id: dataInteration;

    Component.onCompleted: {
        DataSettings.LoadCAServerInfo();
    }

    function send(reqType, params, callbackmsg, callbackprogress,direct) {
        var qid = internal.getNextQid();
        var request = {
            qid: qid,
            reqType: reqType,
            params: params,
            requested: false,
            callbackmsg: callbackmsg,
            callbackprogress: callbackprogress,
//            direct: direct
        }

        internal.requests[qid] = request;
        internal.send(request);
        return request;
    }

    function cancel(qid) {
        var request = internal.requests[qid];
        if (request && request.requested === true) {
            internal.cancel({
                                qid: qid
                            });
        }
        delete internal.requests[qid];
    }

    QtObject {
        id: internal;
        property int qid: 0;

        property var requests: ({});

        function getNextQid() {
            return qid++;
        }

        function send(request) {
            // 发送请求
            DI.send(request.qid,request.reqType,request.params);
            request.requested = true;
        }

        function cancel(qid) {
            // 取消请求
            DI.cancel(request.qid);
            request.requested = true;
        }

    }

    Connections {
        target: DI

        onMessage: {

            // 接受到信息，根据接收到的数据的qid找到对应的请求，处理回调方法

            var err = data.err;
            var desc = data.desc
            var source = data.source;
            var isEof = data.eof;
            var datacontent = data.data
            var request = internal.requests[qid];
            if (request) {
                request.callbackmsg && request.callbackmsg(qid,data);
                if (request.params.sub !== 1) {
                    delete internal.requests[qid];
                }
            }
        }

        onProgress: {

            // 接受到信息，根据接收到的数据的qid找到对应的请求，处理回调方法

            var text = data.text;
            var pos = data.pos
            var total = data.total;

            var request = internal.requests[qid];
            if (request) {
                request.callbackprogress && request.callbackprogress(qid,data);

            }
        }

    }
}
