class _videoTouchData {
    // 构造函数
    constructor() {
        this.urlName = "";
        this.htmlDiv = "";
        this.idType = "";
        this.isRefresh = false;
        this.isTouch = false;
        this.isTouchMove = false;
        this.isTouchEnd = false;
        this.isTouchStart = false;
    }
    // 方法
    videoTouchTask (urlName, htmlDiv, idType) {
        this.idType = idType;
        this.htmlDiv = htmlDiv;
        this.urlName = urlName;
        if (!isUserPage(urlName)) {
            console.log("当前页面不是" + urlName);
            return;
        }
        var div = null;
        // 根据不同的类型, 获取页面中的某个元素
        switch (idType) {
            case "class":
                div = document.getElementsByClassName(htmlDiv);
                break;
            case "id":
                div = document.getElementById(htmlDiv);
                break;
            case "tag":
                div = document.getElementsByTagName(htmlDiv);
                break;
            default:
                break;
        }
        // 如果没有找到元素就返回
        if (div == null || (div.length === 0 && idType !== "id")) {
            console.log("没有找到元素" + htmlDiv);
            return;
        }

        // 添加事件监听器的函数
        function addEventListenersToElement (element) {
            element.addEventListener("click", function (event) {
                event.preventDefault();
            });
            element.addEventListener("contextmenu", function (event) {
                event.preventDefault();
            });
            element.addEventListener("touchstart", function (event) {
                event.preventDefault();
            });
            element.addEventListener("touchmove", function (event) {
                event.preventDefault();
            });
            element.addEventListener("touchend", function (event) {
                event.preventDefault();
            });
        }

        // 如果找到元素, 则阻止该元素类所有的默认事件
        if (div instanceof HTMLCollection || div instanceof NodeList) {
            Array.from(div).forEach(addEventListenersToElement);
        } else {
            addEventListenersToElement(div);
        }

        console.log("已经阻止了" + htmlDiv + "的默认事件");
    }


    getisRefresh () {
        if (this.isTouchStart && this.isTouchMove && this.isTouchEnd) {
            this.isRefresh = true;
        }
        return this.isRefresh;
    }
    refreshVideo () {
        this.isRefresh = false;  // 重置
        this.isTouch = false;  // 重置
        this.isTouchMove = false;  // 重置
        this.isTouchEnd = false;  // 重置
        this.isTouchStart = false;  // 重置
        this.videoTouchTask(this.urlName, this.htmlDiv, this.idType);
    }

}
