class _videoStopButDown {
    constructor(options = {}) {
        this.video = null;
        this.button = null;
        this.step = options.step || 0.1;
        this.interval = options.interval || 500;
        this.threshold = options.threshold || 1;
        this.onStateChange = options.onStateChange || null;
        this.bufferInterval = null;
        this.originalTime = 0;
        this.Count = 0;
        this.state = 'idle'; // idle, buffering, paused, finished
    }

    openButton () {
        const btn = document.createElement('button');
        btn.id = 'chai_bilibil';
        btn.textContent = '双击开始缓冲';
        Object.assign(btn.style, {
            position: 'absolute',
            left: '100px',
            top: '100px',
            background: '#ccc',
            color: '#000',
            zIndex: '9999',
            cursor: 'pointer'
        });
        btn.disabled = false;
        document.body.appendChild(btn);
        this.button = btn;

        // 优化后的拖动逻辑
        let isDragging = false;
        let offsetX, offsetY;

        const handleMouseDown = (e) => {
            isDragging = true;
            offsetX = e.clientX - btn.getBoundingClientRect().left;
            offsetY = e.clientY - btn.getBoundingClientRect().top;
            btn.style.cursor = 'move';
        };

        const handleMouseMove = (e) => {
            if (!isDragging) return;
            btn.style.left = `${e.clientX - offsetX}px`;
            btn.style.top = `${e.clientY - offsetY}px`;
        };

        const handleMouseUp = () => {
            isDragging = false;
            btn.style.cursor = 'pointer';
        };

        // 添加事件监听
        btn.addEventListener('mousedown', handleMouseDown);
        document.addEventListener('mousemove', handleMouseMove);
        document.addEventListener('mouseup', handleMouseUp);
    }

    _updateButton (text, disabled) {
        if (this.button) {
            this.button.textContent = text;
            this.button.disabled = disabled;
        }
    }

    _emitState () {
        if (typeof this.onStateChange === 'function') {
            this.onStateChange(this.state);
        }
    }

    stopBuffer (resetTime = false) {
        this.state = 'paused';
        if (this.bufferInterval) {
            clearInterval(this.bufferInterval);
            this.bufferInterval = null;
        }
        this._updateButton('继续缓冲', false);
        this._emitState();
        if (resetTime) {
            this.video.currentTime = this.originalTime + 10;
        }
    }

    startBuffer () {
        if (this.bufferInterval) return;
        this.state = 'buffering';
        this._updateButton('停止缓冲', false);
        this._emitState();

        this.bufferInterval = setInterval(() => {
            const buffered = this.video.buffered;
            if (buffered.length > 0) {
                const end = buffered.end(buffered.length - 1);
                if (this.video.currentTime < end - this.threshold) {
                    this.video.currentTime = Math.min(this.video.currentTime + this.step, end - 10);
                } else {
                    this.stopBuffer();
                    this._updateButton('缓冲已完成', false);
                    this.state = 'finished';
                    this._emitState();
                }
            }
        }, this.interval);
    }

    toggleBuffer () {
        if (this.Count <= 2) {
            this.Count++;
        }
        let timer = setTimeout(() => {
            if (this.Count === 1) {
                // 单击逻辑,触发拖动
                this.video.currentTime = this.originalTime;
                clearTimeout(timer);
            } else if (this.Count == 2) {
                // 双击逻辑,触发缓冲
                if (this.state === 'buffering') {
                    this.stopBuffer();
                } else if (this.state === 'paused' || this.state === 'idle') {
                    this.startBuffer();
                } else {
                    this.startBuffer();
                }
                clearTimeout(timer);
            }
            this.Count = 0;
        }, 250);
    }

    onPause () {
        if (this.state != 'buffering') {
            this.state = 'paused';
        }
    }

    onPlay () {
        this.state = 'paused';
        this.stopBuffer(true);
    }

    StopButDownTask (video_class) {
        this.openButton();
        // 根据不同的类型, 获取页面中的某个元素
        this.video = document.querySelector(video_class);
        if (!this.video) return;
        this.video.addEventListener('pause', () => this.onPause());
        this.video.addEventListener('play', () => this.onPlay());
        this.button.addEventListener('click', () => this.toggleBuffer());
        this.button.disabled = false;
        this.originalTime = this.video.currentTime;
    }
}