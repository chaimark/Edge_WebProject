class _autoTouchClass {

    // 初始化点击次数和定时器
    maxClicks = 1000;
    interval = 500;
    clickinterval = null;

    // 🔧 辅助函数：执行单次点击
    _clickButton (selector) {
        let button = document.querySelector(selector);
        if (button) {
            button.click();
            return true;
        } else {
            console.log(`❌ 未找到按钮: ${selector}`);
            return false;
        }
    }

    // 🎨 创建自定义输入弹窗
    _createInputDialog (callback) {
        // 创建弹窗容器
        const dialog = document.createElement('div');
        dialog.id = 'custom-input-dialog';
        dialog.innerHTML = `
            <div style="
                position: fixed;
                top: 0;
                left: 0;
                width: 100%;
                height: 100%;
                background: rgba(0,0,0,0.5);
                display: flex;
                justify-content: center;
                align-items: center;
                z-index: 10000;
                font-family: Arial, sans-serif;
            ">
                <div style="
                    background: white;
                    padding: 30px;
                    border-radius: 10px;
                    box-shadow: 0 4px 20px rgba(0,0,0,0.3);
                    min-width: 300px;
                    text-align: center;
                ">
                    <h3 style="margin: 0 0 20px 0; color: #333;">请输入点击参数</h3>
                    <p style="color: #666; margin: 0 0 15px 0; font-size: 14px;">
                        格式: 次数,间隔毫秒<br>
                        例如: 10,500 表示点击10次，每次间隔500毫秒
                    </p>
                    <input type="text" id="custom-input-value" 
                        value="1000,500" 
                        style="
                            width: 100%;
                            padding: 10px;
                            border: 2px solid #ddd;
                            border-radius: 5px;
                            font-size: 16px;
                            margin-bottom: 20px;
                            box-sizing: border-box;
                        "
                        placeholder="次数,间隔毫秒">
                    <div style="display: flex; gap: 10px; justify-content: center;">
                        <button id="custom-input-ok" style="
                            padding: 10px 20px;
                            background: #4CAF50;
                            color: white;
                            border: none;
                            border-radius: 5px;
                            cursor: pointer;
                            font-size: 14px;
                        ">确定</button>
                        <button id="custom-input-cancel" style="
                            padding: 10px 20px;
                            background: #f44336;
                            color: white;
                            border: none;
                            border-radius: 5px;
                            cursor: pointer;
                            font-size: 14px;
                        ">取消</button>
                    </div>
                </div>
            </div>
        `;

        document.body.appendChild(dialog);

        // 绑定事件
        const input = document.getElementById('custom-input-value');
        const okBtn = document.getElementById('custom-input-ok');
        const cancelBtn = document.getElementById('custom-input-cancel');

        okBtn.onclick = () => {
            const value = input.value.trim();
            document.body.removeChild(dialog);
            callback(value);
        };

        cancelBtn.onclick = () => {
			clearInterval(this.clickinterval);
            document.body.removeChild(dialog);
            callback(null);
        };

        // 回车键确认
        input.onkeypress = (e) => {
            if (e.key === 'Enter') {
                okBtn.click();
            }
        };

        // 聚焦输入框
        input.focus();
        input.select();
    }
    // 🎯 修改后的函数：支持按钮名字、间隔周期、点击总次数
    StatrTouch_Chick (selector) {
        this._createInputDialog((userInput) => {
            if (!userInput) {
                console.log('❌ 用户取消了输入');
                return;
            }
            // 🔍 解析用户输入
            const params = userInput.split(',');
            if (params.length !== 2) {
                alert('❌ 输入格式错误！请使用: 次数,间隔毫秒');
                return;
            }
            this.maxClicks = parseInt(params[0].trim());
            this.interval = parseInt(params[1].trim());
            // ✅ 验证输入
            if (isNaN(this.maxClicks) || isNaN(this.interval) || this.maxClicks <= 0 || this.interval < 0) {
                alert('❌ 输入无效！次数必须大于0，间隔必须大于等于0');
                return;
            }

            let clickCount = 0;

            console.log(`🚀 开始点击任务 - 选择器: ${selector}, 间隔: ${this.interval}ms, 次数: ${this.maxClicks}`);

            // 如果只需要点击一次，直接返回
            if (this.maxClicks <= 1) {
                this._clickButton(selector);
                console.log(`✅ 点击了 1 次`);
                return;
            }
            // 设置定时器进行后续点击
            this.clickinterval = setInterval(() => {
                if (clickCount >= this.maxClicks) {
                    console.log(`✅ 点击了 ${clickCount + 1} 次`);
                    clearthis.interval(this.clickinterval);
                    return;
                }

                if (this._clickButton(selector) == true) {
                    clickCount++;
                    console.log(`✅ 点击了 ${clickCount} 次`);
                } else {
                    // 点击失败结束整个流程
                    console.log(`❌ 点击失败，已点击 ${clickCount} 次`);
                    clearthis.interval(this.clickinterval);
                    return;
                }
            }, this.interval);
        });
    }
    Init () {
        // 如果有正在运行的，先清除
        clearInterval(this.clickinterval);
        // 初始化点击次数和定时器
        this.maxClicks = 1000
        this.interval = 500
        this.clickinterval = null;
    }
}

var AutoChick = new _autoTouchClass();
function AutoTouch_wait () {
    if (!isUserPage("live.bilibili")) {
        console.log("当前页面不是" + "live.bilibili");
        return;
    }
    chrome.runtime.onMessage.addListener(function (request, sender, sendResponse) {
        if (request.action === 'runAutoTouchButtonScript') {
            sendResponse('run ok.');
            let timer = setTimeout(() => {
                AutoChick.Init();
                AutoChick.StatrTouch_Chick(".like-btn");
                clearTimeout(timer);
            }, 1000);
        }
    });
}

