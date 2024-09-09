//import * as MOCKED_SESSION from './datasets/fullbsn.json';
//import * as MOCKED_SESSION from './datasets/maria-dataset.json';
import { Logger } from '@nestjs/common';
import { BsnClientCallback } from 'src/bsn-client/bsn-client.service';
// import * as maria_01 from './kinematics-maria-01.json';
// import * as maria_02 from './kinematics-maria-02.json';
// import * as knee_01 from './kinematics-knee-movement.json';
// import * as heartrate_01 from './kinematics-knee-movement.json';
// import * as heartrate_02 from './kinematics-knee-movement.json';

const datasets = ['maria-dataset.json','mathias-iso.json','mathias-02.json','mathias-02.json']

const fs = require('fs');
const path = require('path');

export class SessionsSimulator {

    public constructor(
    ) {
        this.readDataset(path.resolve(__dirname,`../assets/datasets/${this.getRandomDataSet()}`));
    }

    private readonly logger = new Logger(SessionsSimulator.name);

    private callback: BsnClientCallback;

    private state = {
        dataSetIndex: 0,
        intervalTimer: null,
        samplingRate: 1,
        offset: 0,
        lastTime: 0,
        sessionStart: 0,
        sessionEnd:0,
        isRunning: false,
        dataSet : null,
    };

    private getRandomDataSet() : string {
        function getRandomInt(max) {
            return Math.floor(Math.random() * Math.floor(max));
          }
        // read dataset
        let dataSetIndex = getRandomInt(datasets.length);
        console.log(datasets);
        console.log(datasets.length);
        console.log(dataSetIndex);
        return datasets[dataSetIndex];
    }

    private readDataset(filename: string) {
        this.logger.log(`reading dataSet from ${filename}`);
        try {
            const fileContent = fs.readFileSync(filename,'utf8').trim();
            this.state.dataSet = JSON.parse(fileContent);
            //this.logger.log(gunzippedContent);
            this.logger.log(`dataSet ${filename} has ${this.state.dataSet.length} samples`)
        }
        catch (error) {
            this.logger.log(error);
        }
    }
    private getOffsetFromSamplingRate(){
        return Math.round(1000 / this.state.samplingRate);
    }

    private updateSession = () => {
        //get current time
        const currentTime = Date.now() - this.state.sessionStart;
        const lastTime = this.state.lastTime;
        //request active event and creat it if it doesnt exist
        // const lastEvent = this.getEvent(false);
        this.logger.log(`updating at , ${currentTime} before was ${lastTime}`);

        //simulate data for the current interval
        let offset = lastTime;
        for (
            offset;
            offset <= currentTime;
            offset += this.getOffsetFromSamplingRate()
        ) {
            this.state.offset = offset;
            const frameToAdd = this.getNextAnimationFrame();
            try {
                this.callback("bionic",{data:frameToAdd});
            } catch (error) {
                this.logger.error(error);
            }
        }

        //ath the end set lasttime to now
        this.state.lastTime = currentTime;
    }

    // private handleIMUData = data => {
    //     this.incomingData += data;
    //     //remove junk beofre a message starts, might happen because of the software architecture
    //     const junkIndexBeforeMessage = this.incomingData.indexOf('{');
    //     if (junkIndexBeforeMessage !== -1)
    //         this.incomingData = this.incomingData.slice(junkIndexBeforeMessage);
    //     //split incomingdata into messages
    //     let messages = String(this.incomingData).split(/\n/, 2);
    //     //if the delimiter was not found just return from routine
    //     while (messages.length !== 1) {
    //         //valid message found -> send it
    //         if (messages[0].length !== 0) {
    //             let messageToSend = null;
    //             try {
    //                 messageToSend=JSON.parse(messages[0]);
    //                 this.mySettings.callback("bionic",messageToSend );
    //             } catch (error) {
    //                 this.logger.error(error);
    //             }
    //             this.incomingData = this.incomingData.slice(messages[0].length + 1)
    //             messages = String(this.incomingData).split(/\n/, 2);
    //         }
    //     }
    // };

    private getNextAnimationFrame() {
        if (++this.state.dataSetIndex >= this.state.dataSet.length)
            this.state.dataSetIndex = 0;
        return  this.state.dataSet[this.state.dataSetIndex];
    }

    start(sessionEnd?: number) {
        if (this.state.intervalTimer != null)
            clearInterval(this.state.intervalTimer);

        if(sessionEnd !== null && sessionEnd !== undefined){
            this.state.sessionEnd = sessionEnd;
            this.logger.log(`WARNING Simulating a full session between ${this.state.sessionStart} and ${this.state.sessionEnd}`)
        }

        //load a new dataset randomly (slows down - i know)
        this.readDataset(path.resolve(__dirname,`../assets/datasets/${this.getRandomDataSet()}`));

        this.state.sessionStart = Date.now();
        this.state.intervalTimer = setInterval(
            this.updateSession.bind(this),
            this.getOffsetFromSamplingRate()*5,
        );
        this.state.isRunning = true;
        this.updateSession();
        this.logger.log(`started new simulator with sessionStart ${this.state.sessionStart}`)
    }

    stop() {
        //intervaltimer
        clearInterval(this.state.intervalTimer);
        this.state.intervalTimer = null;
        //trigger update for getting the rest of the data up to the end
        this.updateSession();
        this.state.isRunning = false;
    }

    setCallback(callBack: BsnClientCallback): void {
        this.callback = callBack;
        this.logger.log(`callback set to ${callBack}`);
    }

    isActive(){
        return this.state.isRunning;
    }

    getSamplingDelayInMs(): number {
        return this.getOffsetFromSamplingRate();
    }

}
