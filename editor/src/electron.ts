// electron imports
const { app, BrowserWindow, ipcMain } = require('electron')
// get system path
const path = require('path');
// import engine code
const core = require('../../engine/build/Release/core.node');
// import file reading
const fs = require('fs')

/**
 * Function to create a window
 */
const createWindow = () => {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            // preload: path.join(__dirname, 'preload.js'),
            nodeIntegration: true,
            nodeIntegrationInWorker: true,
            contextIsolation: false,
            enableRemoteModule: true,
            devTools: true
        }
    })

    win.loadFile(path.join(__dirname, './index.html'));
}

/**
 * When the app is ready, create a new window
 */
app.whenReady().then(() => {
    createWindow()

    app.on('activate', () => {
        if (BrowserWindow.getAllWindows().length === 0) {
            createWindow();
        }
    });
});

/**
 * Handle window closing
 */
app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }

    process.exit(0);
});

// ENGINE STUFF
try {
    console.log(core.checkEngineStatus(3));
}
catch (e) {
    console.error('error connecting to engine: ', e);
    process.exit(1);
}

// update OpenGL Window
const update = (delta) => {
    if(!core.rendererShuttingDown()) {
        core.updateRenderer();
    }
    else {
        core.shutDownRenderer();
    }
}

// length of a tick in milliseconds
const fps = 30;
let tickLengthMs = 1000 / fps;

/* renderLoop related variables */
// timestamp of each loop
let previousTick = Date.now();

// number of times gameLoop gets called
let actualTicks = 0

const renderLoop = function () {
    // get the time now and number of ticks
    const now = Date.now();
    actualTicks++;

    // update when allowed to
    if (previousTick + tickLengthMs <= now) {
        const delta = (now - previousTick) / 1000;
        previousTick = now;

        update(delta);

        if(core.rendererShuttingDown()) {
            return;
        }

        // console.log('delta', delta, '(target: ' + tickLengthMs +' ms)', 'node ticks', actualTicks);
        actualTicks = 0;
    }

    // blend setImmediate (which is accurate) and setTimeout (which uses less CPU) to have accurate update loop
    if (Date.now() - previousTick < tickLengthMs - 16) {
        setTimeout(renderLoop);
    } else {
        setImmediate(renderLoop);
    }
}

// method to create render loop
const startRenderLoop = () => {
    // init OpenGL
    core.createRenderer();

    // begin the game loop!
    renderLoop();
}
// ENGINE STUFF

/**
 * Handle async message received from client
 */
ipcMain.on('asynchronous-message', (event, arg) => {
    console.log('got message', arg);

    // handle invalid message
    if(!arg || !arg.name) {
        console.error('invalid message format: ', arg);
        return;
    }

    // respond to valid messages
    switch (arg.name) {
        // when client asks for ping
        case 'ping':
            console.log('got ping');

            const reply = {
                name: 'pong',
                data: {
                    "pingTime": Date.now() - arg.data.createdAt,
                    "createdAt": Date.now()
                }
            };

            event.reply('asynchronous-reply', reply);

            break;
        case 'get-frame':
            const imagePath: string = path.join(__dirname, '../../engine/frame.png') || "";
            console.log('image path 1', imagePath);
            fs.readFile(imagePath, (err, data) => {
                // send 'undefined' data if error reading image
                if (err || !data) {
                    const reply = {
                        name: 'image-data',
                        status: 'failure',
                        data: { }
                    };

                    event.reply('asynchronous-reply', reply);

                    return;
                }

                console.log('image path 2', imagePath);

                let imageData;

                try {
                    imageData = new Buffer(data).toString('base64');
                }
                catch (e) {
                    const reply = {
                        name: 'image-data',
                        status: 'failure',
                        data: { }
                    };

                    event.reply('asynchronous-reply', reply);

                    return;
                }

                const reply = {
                    name: 'image-data',
                    status: 'success',
                    data: {
                        imageType: 'png',
                        imageEncoding: 'base64',
                        imageData: imageData
                    }
                };

                event.reply('asynchronous-reply', reply);
            });
            break;
        case 'start-renderer':
            startRenderLoop();
            break;
        default:
            console.warn('unknown message: ', arg);
            break;
    }
});