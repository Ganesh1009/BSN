import { Injectable, Logger } from '@nestjs/common';
import { Socket } from 'net';
import { finished } from 'stream';

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

// import { ClientTCP } from '@nestjs/microservices';
export interface BsnClientCallback {
    (type: string, data: object): void;
}

class BsnClientSettings {
    ip: string;
    port: number;
    callback: BsnClientCallback;
}

const initialStates = ['mode jsonmars', 'init', 'start'];

@Injectable()
export class BsnClientService {
    constructor() {
        this.setHandlers();
    }
    private readonly logger = new Logger(BsnClientService.name);

    private connectionstates = initialStates.slice();
    private currentstate = 0;
    private eepromsread = false;
    private sessionconfigured = false;
    private sessionConfigCommands: string[] = [];
    private sessionState = 0;
    private incomingData = String();
    private tcpClient: Socket = new Socket();
    private sensors: number[] = [];
    private tryReconnect = false;
    private waitingForConnection = true;

    private mySettings: BsnClientSettings = {
        ip: '127.0.0.1',
        port: 5001,
        callback: null,
    };

    private handleInitialStates = data => {
        //get to next state if there is one
        if (this.currentstate < this.connectionstates.length) {
            if (this.eepromsread === false) {
                this.connectionstates = initialStates.slice();
                //now add the read eeprom commands to the state machine
                for (const i in this.sensors) {
                    this.connectionstates.unshift(
                        'eeprom ' + (this.sensors[i] + 0x40).toString(16),
                    );
                    this.logger.log(
                        'eeprom ' +
                            (this.sensors[i] + 0x40).toString(16) +
                            ' added',
                    );
                }
                this.eepromsread = true;
            }

            //go through the different states:
            this.logger.log('Received: ' + data.slice(0, 10));

            const dataAsString = String(data);

            //if last message was not nacked go to next state
            if (dataAsString.indexOf('_nack') !== -1) {
                this.logger.log('nack ===>>> resend');
                this.currentstate--;
            }
            if (this.currentstate == 0) {
                this.logger.log(
                    "statechange 'detect' ===>>> " +
                        this.connectionstates[this.currentstate],
                );
            } else
                this.logger.log(
                    "statechange '" +
                        this.connectionstates[this.currentstate - 1] +
                        "' ===>>> '" +
                        this.connectionstates[this.currentstate] +
                        "'",
                );
            this.sendMessage(this.connectionstates[this.currentstate++]);
        } else {
            if(this.sessionState < this.sessionConfigCommands.length ) {
                let nextState = this.sessionConfigCommands[this.sessionState++];
                this.sendMessage(nextState);

            }
            this.handleIMUData(data);
        }
    };
    private handleIMUData = data => {
        this.incomingData += data;
        //remove junk beofre a message starts, might happen because of the software architecture
        const junkIndexBeforeMessage = this.incomingData.indexOf('{');
        if (junkIndexBeforeMessage !== -1)
            this.incomingData = this.incomingData.slice(junkIndexBeforeMessage);
        //split incomingdata into messages
        let messages = String(this.incomingData).split(/\n/, 2);
        //if the delimiter was not found just return from routine
        while (messages.length !== 1) {
            //valid message found -> send it
            if (messages[0].length !== 0) {
                let messageToSend = null;
                try {
                    messageToSend=JSON.parse(messages[0]);
                    this.mySettings.callback("bionic",messageToSend );
                } catch (error) {

                }
                this.incomingData = this.incomingData.slice(messages[0].length + 1)
                messages = String(this.incomingData).split(/\n/, 2);
            }
        }
    };

    private parseMessage = data => {
        this.handleInitialStates(data);
    };

    private checkForConnection = data => {
        // convert to string to search for ack
        let dataAsString = String(data);
        // find "detect_ack" in response
        const msgStart = dataAsString.indexOf('detect_ack');
        // parse the detect call
        if (msgStart !== -1) {
            // detect_ack received -> clear wait flag
            this.waitingForConnection = false;
            this.sensors = [];
            dataAsString = dataAsString.substring(msgStart);
            //get sensor information out of the detect_ack message
            const splitAtEqualSign = dataAsString.split('=');
            if (splitAtEqualSign[1] != undefined) {
                const i2cAddresses = splitAtEqualSign[1];
                const addresses = i2cAddresses.split(',');
                addresses.forEach(entry => {
                    if (parseInt(entry, 16) < 0x40)
                        this.sensors.push(parseInt(entry, 16));
                });
                this.logger.log(this.sensors);
            }
        } else {
            // no detect_ack -> see if it was a _nack -> try to reconnect
            if (dataAsString.indexOf('_nack') !== -1) {
                this.logger.log('detect_nack received');
                // close client and restart again...
                this.tryReconnect = true;
                this.tcpClient.end(); // disconnect client
                this.waitingForConnection = true;
                // tell about disconnected client
                return false;
            }
        }
        return true;
    };

    setHandlers() {
        this.logger.log('attaching the handlers to the tcpclient here');
        //attach handlers
        this.tcpClient.on('close', () => {
            this.logger.log('Connection closed');
            if (this.tryReconnect) {
                this.tryReconnect = false;
                sleep(100);
                this.startMeasurement(this.sessionConfigCommands);
            }
            // delete reference to client
            this.tcpClient.unref();
            this.waitingForConnection = true;
            if(this.mySettings.callback !== null)
                this.mySettings.callback("update",{message:"closed"});
        });

        // attach timeout handler
        this.tcpClient.on('timeout', data => {
            if (this.waitingForConnection) {
                this.logger.log('timeout on detect');
                this.tcpClient.end(); // disconnect client
                this.waitingForConnection = true;
                if(this.mySettings.callback !== null)
                    this.mySettings.callback("update",{message:"timeout"});
            }
        });

        // attach error handler
        this.tcpClient.on('error', err => {
            this.logger.error(err);
            this.tcpClient.end(); // disconnect client
            this.waitingForConnection = true;
            if(this.mySettings.callback !== null)
                this.mySettings.callback("error",{message:err});
        });

        // message handler for incoming messages
        this.tcpClient.on('data', data => {
            // message received so clear waitflag
            if (this.waitingForConnection)
                if (this.checkForConnection(data) == false) return;
            this.parseMessage(data);
        });
    }

    sendMessage(message: string): any {
        if (this.isConnected()) {
            this.tcpClient.write(message);
        } else {
            this.logger.warn('tcp client not connected');
        }
    }

    startMeasurement(configCommands: string[]): any {
        if (this.isConnected() === false) {
            this.sessionConfigCommands = configCommands;
            try {
                //connect client here
                this.tcpClient.connect(
                    this.mySettings.port,
                    this.mySettings.ip,
                    () => {
                        this.tcpClient.setTimeout(3000);
                        this.logger.log(
                            `connected to ${this.mySettings.ip}:${this.mySettings.port}`,
                        );
                        this.waitingForConnection = true;
                        this.tcpClient.write('detect');
                    },
                );
            } catch (error) {
                try{
                    this.mySettings.callback("error",{message:error});
                }catch{
                    this.logger.error(error);
                }
            }
        }
        this.logger.log(`Started Measurement`);
    }
    startCalibration(): any {
        this.logger.log(`Started Calibration`);
    }
    stopMeasurement(): any {
        if (this.isConnected() === true) {
            this.tcpClient.end('stop');
            this.waitingForConnection = true;
        }
        this.currentstate = 0;
        this.sessionState = 0;
        this.logger.log(`Stopped Measurement`);
    }
    setCallback(callBack: BsnClientCallback): void {
        this.mySettings.callback = callBack;
        this.logger.log(`callback set to ${this.mySettings.callback}`);
    }
    removeCallback(): void {
        this.mySettings.callback = null;
        this.logger.log(`callback set to ${this.mySettings.callback}`);
    }
    isActive(): boolean {
        if (this.currentstate < this.connectionstates.length) return true;
        else return false;
    }
    isConnected(): boolean {
        if (this.tcpClient === undefined || this.tcpClient.destroyed) {
            return false;
        } else {
            return !this.waitingForConnection;
        }
    }
    getSamplingDelayInMs(): number {
        return 10;
    }
    getOffsetInMs(): number {
        return 0;
    }
}
