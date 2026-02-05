class _autoPayClass {
    constructor(options = {}) {
        this.tagTime = "2026-02-03T06:28:00";
        this.selectors = "undefine";
    }
    // 🔍 验证日期时间格式
    isValidDateTime (dateTimeStr) {
        const date = new Date(dateTimeStr);
        return date instanceof Date && !isNaN(date);
    }

    // 尝试结算函数
    attemptCheckout () {
        let checkoutButton = null;
        checkoutButton = document.querySelector(this.selectors);
        if (checkoutButton) {
            console.log('✅ 找到结算按钮，选择器：', this.selectors, '文本内容：', checkoutButton.textContent);
        }

        if (checkoutButton) {
            // 模拟点击
            checkoutButton.click();
        } else {
            console.log('🔍 未找到结算按钮，继续尝试...');
        }
    }
    // 监控函数
    AutoClickPayElement () {
        if (isUserPage("buy.tmall")) {
            return;
        }
        // 目标时间：2026年2月5日 22:00:00
        const targetDate = new Date(this.tagTime);
        console.log('🛒 自动购买脚本已启动...');
        console.log('📅 目标时间：2026年2月5日 22:00:00');
        console.log('⏰ 当前时间：', new Date().toLocaleString());

        const checkInterval = setInterval(() => {  // 箭头函数不会创建新this
            const now = new Date();
            const timeDiff = targetDate.getTime() - now.getTime();
            // 每秒输出一次倒计时（移除 % 10 === 0 的条件）
            const remainingSeconds = Math.ceil(timeDiff / 1000);
            console.log('⏱️ 当前时间：', now.toLocaleString(), '⏳ 剩余时间：', remainingSeconds, '秒');

            if (timeDiff <= 0) {
                console.log('🎯 到达目标时间，开始执行购买操作！');
                clearInterval(checkInterval);
                this.attemptCheckout();
            }
        }, 500);

        // 页面加载完成后自动启动
        if (document.readyState === 'loading') {
            document.addEventListener('DOMContentLoaded', function () {
                console.log('📄 页面加载完成，自动购买脚本已激活');
            });
        } else {
            console.log('📄 页面已加载，自动购买脚本已激活');
        }
    }

    // 接口函数，用于开启定时任务
    Set_ButtonAndRunTask (ButtonClass) {
        this.selectors = ButtonClass;
        // 🎯 弹出输入框让用户输入时间
        const userInput = prompt('请输入目标时间 (格式: 2026-02-05T22:00:00):', '2026-02-05T22:00:00');
        // ✅ 验证输入格式
        if (userInput && this.isValidDateTime(userInput)) {
            this.tagTime = userInput;
            alert(`✅ 已设置目标时间: ${new Date(userInput).toLocaleString()}` + "\n🛒 自动购买脚本已启动!");
            this.AutoClickPayElement();
        } else {
            // ❌ 输入无效时使用默认值
            this.tagTime = "2026-02-03T06:28:00";
            alert('⚠️ 输入格式无效，使用默认时间');
        }
    }
}
