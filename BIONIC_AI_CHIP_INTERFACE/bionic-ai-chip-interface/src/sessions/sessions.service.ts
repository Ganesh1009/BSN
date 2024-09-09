/* eslint-disable @typescript-eslint/camelcase */
import { Injectable, Logger } from '@nestjs/common';
import { SessionStatus, SessionConfig, SessionSimulationConfig } from './sessions.dto';
import { CreateBSNDataDTO, CreateCategory, CreateDatum, CreateEvent } from './dto/create-bsndata.dto';

import { BsnClientService } from '../bsn-client/bsn-client.service';
import { SessionsSimulator } from './sessions.service.simulator';
import { BSNSessionDTO } from './dto/bsnsession.dto';
import { BSNDataDTO } from './dto/bsndata.dto';
import { ConfigService } from '@nestjs/config';

const fs = require('fs');
const path = require('path');
const zlib = require('zlib');


@Injectable()
export class SessionsService {
    public constructor(
        private bsnClient: BsnClientService,
        private configService: ConfigService
    ) {
        //read all exisitng sessions
        this.readAllSessionFiles()
    }
    private readonly logger = new Logger(SessionsService.name);

    private mySessionStatus: SessionStatus = {
        isActive: false,
    };

    private use_on_change_encoding = false;
    private smoothedEventLevel = 0;
    private testOut=0;
    private sessionSimulator: SessionsSimulator = new SessionsSimulator();
    private storagePath = this.configService.get<string>('BIONIC_STORAGE_PATH') + '/storage/';
    private mySessionConfig: SessionConfig = new SessionConfig();
    private myActiveSession: CreateBSNDataDTO = new CreateBSNDataDTO();
    private mySessionList: CreateBSNDataDTO[] = [
        {
            date: Date.UTC(1982, 10, 20, 0, 0, 0, 0),
            task: 'i am just a dummy entry',
            timezone: Intl.DateTimeFormat().resolvedOptions().timeZone,
            events: [],
        } as CreateBSNDataDTO,
    ];

    private getSamplingRate(){
        if(this.bsnClient.isActive() && this.bsnClient.isConnected())
            return Math.round(1000/this.bsnClient.getSamplingDelayInMs());
        else
            return Math.round(1000/this.sessionSimulator.getSamplingDelayInMs());
    }


    private getElementInCategory(category: CreateCategory, elementLabel: string): CreateDatum {
        // find kinematics
        let segmentToFind = category.data.find(
            element => element.name === elementLabel,
        );
        // create if doens't exist
        if (segmentToFind === null || segmentToFind === undefined) {
            category.data.push({
                name: elementLabel,
                sampling_rate: this.getSamplingRate(),
                number_of_samples: 0,
                offset: 0,
                samples: [],
            } as CreateDatum);
            this.logger.log(`adding ${elementLabel} to ${JSON.stringify(category.data)}`);
            segmentToFind = category.data.find(
                element => element.name === elementLabel,
            );
        }
        //return
        return segmentToFind;
    }

    /*
    [{
        "kinematics": {
            "Sternum":{"ts":52295,"tmp":55.044117,"a":[-6.732418,-0.718022,9.102811],"g":[0.087281,0.334277,-0.019916],"m":[-0.272024,-0.963728,0.499776],"p":[0.00612,0.233969,0.785844],"q":[-0.580634,-0.39854,0.109372,-0.701475]},
            "RightUpperArm":{"ts":52295,"tmp":47.394119,"a":[-10.334938,1.37929,6.143706],"g":[-0.270358,-0.211616,-0.118447],"m":[-0.43488,0.461472,0.943768],"p":[0.29869,0.199833,1.046794],"q":[0.476845,0.271763,0.658272,0.51521]},
            "LeftUpperArm":{"ts":52295,"tmp":45.120586,"a":[11.029625,3.659584,-1.895455],"g":[-0.431291,-0.083659,-0.067708],"m":[-0.635816,-0.130464,0.786664],"p":[0.161656,0.487971,1.04411],"q":[0.757052,-0.165889,0.046888,0.630202]},
            "LeftLowerArm":{"ts":52295,"tmp":45.544117,"a":[-10.791602,-4.839503,0.083708],"g":[0.296821,-0.073433,-0.337866],"m":[0.2584,-0.916344,0.573104],"p":[0,0,0],"q":[0,0,0,0]},
            "LeftWrist":{"ts":52295,"tmp":48.349998,"a":[-9.379631,-4.942768,5.695071],"g":[0.324713,-0.304568,-0.206173],"m":[1.12048,-0.496432,0.65472],"p":[0,0,0],"q":[0,0,0,0]},
            "Pelvis":{"ts":52295,"tmp":48.167648,"a":[8.662389,1.043792,6.882767],"g":[0.036529,-0.159124,0.034605],"m":[1.145184,0.634432,0.93496],"p":[0.068151,0.261805,0.677081],"q":[0.685834,0.21743,0.042471,0.693218]},
            "LeftUpperLeg":{"ts":52295,"tmp":46.752941,"a":[8.941381,-5.022079,-2.45707],"g":[-0.157925,-0.078076,0.388761],"m":[-0.762352,-0.564376,0.904704],"p":[-0.03214,0.322888,0.609735],"q":[0.73423,-0.184302,0.484445,0.438467]},
            "LeftLowerLeg":{"ts":52295,"tmp":46.988235,"a":[-3.602408,-8.507144,-3.87581],"g":[0.097051,0.038978,0.040467],"m":[-0.941184,0.529104,0.440376],"p":[0.25666,0.541748,0.413578],"q":[0.615966,0.239197,-0.063919,0.747853]},
            "LeftFoot":{"ts":52295,"tmp":44.682354,"a":[3.13212,-2.29916,8.875931],"g":[-0.212957,0.067257,-0.038922],"m":[-0.095576,-0.553784,0.529944],"p":[0.054907,0.428398,0.025976],"q":[0.672149,0.187284,0.240269,0.674841]},
            "RightUpperLeg":{"ts":52295,"tmp":47.402943,"a":[-8.69121,-5.363138,0.833011],"g":[-0.079893,0.207279,-0.364212],"m":[-0.733584,0.188448,1.239408],"p":[0.033265,0.153687,0.60327],"q":[0.596556,-0.302568,0.472354,0.573981]},
            "RightLowerLeg":{"ts":52295,"tmp":46.164707,"a":[-2.786668,-9.148877,2.912879],"g":[-0.068222,-0.125559,-0.016178],"m":[0.007296,0.021744,-1.15692],"p":[0.404627,0.220496,0.433897],"q":[0.636113,0.258253,-0.039815,0.726003]},
            "RightFoot":{"ts":52295,"tmp":44.861763,"a":[-1.000075,-4.498108,8.012805],"g":[-0.140572,-0.130821,-0.013686],"m":[-0.40128,-0.5394,0.536696],"p":[0.200671,0.131653,0.0423],"q":[0.72513,0.043107,0.072038,0.683475]},
            "Head":{"ts":52295,"tmp":51.097057,"a":[-1.03583,-12.008983,-2.614561],"g":[-0.087766,0.156015,0.000142],"m":[0.03596,-1.093032,-0.059712],"p":[0.311371,0.383153,1.145183],"q":[-0.595684,-0.351681,0.084067,-0.717226]}
        },
       f "physiological":{
            "heartrate":{"ts":52295,"hr":91,"s":""}
        },
        "ergonomic":{
            "owas":{"ts":52295,"scores":[3,2,1,4,3,2,1,3]}
        },
        "kinetics":{
            "carriedload":{"ts":52295,"carriedload":-0.000243},
            "insoles":[
                {"ts":52295,"tf":476,"pa":[9.5,6.25,6.25,5.5,2.5,1.25,1.25,1.25,1.25,1.5,0.75,0.5,0.75,1.25,0.5,0.5],"cop":[-0.193894,-0.024112],"b":100},
                {"ts":52295,"tf":506,"pa":[7,9,5.25,6,2,1,2.5,1.75,2.25,1.5,0.75,1,1.5,0.5,0.25,0.5],"cop":[-0.170054,-0.042402],"b":100}
            ]
        }
    }]
    }*/

    private readAllSessionFiles() {
        let session: CreateBSNDataDTO;
        this.logger.log(`reading ${this.storagePath}`);
        if (!fs.existsSync(this.storagePath)) {
            fs.mkdirSync(this.storagePath, {recursive:true});
        }
        try {
            fs.readdirSync(this.storagePath).forEach(file => {
                if (path.extname(file) === '.bdo') {
                    this.logger.log(file);
                    const fileContent = fs.readFileSync(path.resolve(this.storagePath, file));
                    const gunzippedContent = zlib.gunzipSync(fileContent);
                    //add to local sessions (might need to change due to high ram usage of this approach)
                    this.mySessionList.push(Object.assign({}, JSON.parse(gunzippedContent)));
                    //this.logger.log(gunzippedContent);
                }
            })
        }
        catch (error) {
            this.logger.log(error);
        }
    }
    private saveSessionToFile(session: CreateBSNDataDTO) {
        this.logger.log(`saving ${session.date} to ${path.resolve(this.storagePath, session.date + '.bdo')}`);
        const gzipped = zlib.gzipSync(JSON.stringify(session), { level: 9 });
        //save it always with max gzip compression
        fs.writeFileSync(
            path.resolve(this.storagePath, session.date + '.bdo'),
            gzipped
        );
    }
    private deleteSessionFile(session: CreateBSNDataDTO) {
        try {
            fs.rmSync(path.resolve(this.storagePath, session.date + '.bdo'));
        }
        catch (error) {
            this.logger.log(error);
        }
    }

    private addNewFrame(category: CreateCategory, kinematicsFrame) {
        for (const [key, value] of Object.entries(kinematicsFrame)) {
            switch (category.name) {
                case "kinematics": {
                    const element: CreateDatum = this.getElementInCategory(category, key);
                    element.number_of_samples++;
                    if (Array.isArray(element.samples))
                        element.samples.push([].concat(value['p']).concat(value['q']));
                }
                    break;
                case "ergonomics": {
                    const element: CreateDatum = this.getElementInCategory(category, key);
                    element.number_of_samples++;
                    if (Array.isArray(element.samples))
                        element.samples.push([].concat(value['scores']));
                }
                    break;
                case "kinetics": {
                    if (key === "carriedload") {
                        const element: CreateDatum = this.getElementInCategory(category,"carried-load");
                        element.number_of_samples++;
                        //rounding to nearest 1kg
                        let valueToAdd = Math.floor(value['carriedload']);
                        if(this.use_on_change_encoding){

                        }else{
                            if (Array.isArray(element.samples))
                                element.samples.push([].concat(valueToAdd));
                        }
                    }
                }
                    break;
                case "physiological": {
                    const element: CreateDatum = this.getElementInCategory(category,"heart-rate");
                    element.number_of_samples++;
                    //rounding to nearest 1kg
                    let valueToAdd = Math.floor(value['hr']);
                    if(this.use_on_change_encoding){

                    }else{
                        if (Array.isArray(element.samples))
                            element.samples.push([].concat(valueToAdd));
                    }
                }
                    break;
                default:
                    this.logger.warn(`not found ${key}`)
                    break;
            }
        }
    }

    private getOrCreateCategory(categories: Array<CreateCategory>, category: string): CreateCategory {
        // find datacategory
        let datacategory = categories.find(
            element => element.name === category,
        );
        // create if doens't exist
        if (datacategory === null || datacategory === undefined) {
            categories.push({
                name: category,
                data: [],
            });
            this.logger.log(`adding ${category} to ${JSON.stringify(categories)}`);
            datacategory = categories.find(
                element => element.name === category,
            );
        }
        //return
        return datacategory;
    }

    private getEvent(createNew: boolean): CreateEvent {
        //see if we need to initialize an event
        if (
            (this.myActiveSession.events.length === 0)  //first session ever
            || ((createNew === true) && (this.myActiveSession.events[this.myActiveSession.events.length - 1].categories.length !== 0)))
            // createNew requested and previous event had not categories
             {
                const timeOfEventRelativeToSessionStart = Date.now() - this.myActiveSession.date;
                this.myActiveSession.events.push({
                    time: timeOfEventRelativeToSessionStart,
                    categories: [],
                    level: 0
                });
                this.logger.log(
                    `created new event at ${timeOfEventRelativeToSessionStart}`,
                );
        } else {
            if (createNew === true) {
                this.myActiveSession.events[this.myActiveSession.events.length - 1].time = Date.now() - this.myActiveSession.date;
            }
        }
        return this.myActiveSession.events[
            this.myActiveSession.events.length - 1
        ];
    }

    private calcAverage(average,newSample){
        const samplesToAverage=45;
        return (average * (samplesToAverage - 1) + newSample ) / samplesToAverage;
    }

    private checkForNewEvent(newLevel: number ) : CreateEvent {
        let newEvent = this.getEvent(false);
        if(this.smoothedEventLevel === 0)
            this.smoothedEventLevel = newLevel;
        this.smoothedEventLevel = this.calcAverage(this.smoothedEventLevel,newLevel);

        // create a new event on level change
        if(Math.floor(this.smoothedEventLevel) !== newEvent.level) {
            this.logger.log(`level changed: ${newLevel} , ${newEvent.level}`);
            if(newEvent.level !== 0) // reuse event if it has not had a valid level
                newEvent = this.getEvent(true);
            newEvent.level = Math.floor(this.smoothedEventLevel);
        }
        return newEvent;
    }

    private handleBSNData(type: string, data: object) {
        if (type === "bionic") {
            if(this.testOut++ === 10)
            {
                this.logger.log(`${type} -> ${JSON.stringify(data)}`)
                this.testOut = 0;
            }

            //ALWAYS DO THIS FIRST -> IT GENERATE NEW EVENT BASED ON OWAS LEVEL
            if (data['data'] != null && data['data']['ergonomic'] != null) {
                const frameToAdd = data['data']['ergonomic'];
                const owasRiskLevel = frameToAdd['owas']['scores'][0];
                //this.logger.log(`owasRiskLevel: ${owasRiskLevel}`);
                //const newLevel = Math.floor(owasRiskLevel * 3 / 4);
                //this.logger.log(`level: ${newLevel} , ${owasRiskLevel}`);
                const lastEvent = this.checkForNewEvent(owasRiskLevel);
                const ergonomicsCategory = this.getOrCreateCategory(lastEvent.categories, 'ergonomics');
                this.addNewFrame(ergonomicsCategory, frameToAdd);
            }
            if (data['data'] != null && data['data']['kinematics'] != null) {
                const lastEvent = this.getEvent(false);
                const frameToAdd = data['data']['kinematics'];
                const kinematicsCategory = this.getOrCreateCategory(lastEvent.categories, 'kinematics');
                this.addNewFrame(kinematicsCategory, frameToAdd);
            }
            if (data['data'] != null && data['data']['kinetics'] != null) {
                const lastEvent = this.getEvent(false);
                const frameToAdd = data['data']['kinetics'];
                const kineticsCategory = this.getOrCreateCategory(lastEvent.categories, 'kinetics');
                this.addNewFrame(kineticsCategory, frameToAdd);
            }
            if (data['data'] != null && data['data']['physiological'] != null) {
                const lastEvent = this.getEvent(false);
                const frameToAdd = data['data']['physiological'];
                const physiologicalCategory = this.getOrCreateCategory(lastEvent.categories, 'physiological');
                this.addNewFrame(physiologicalCategory, frameToAdd);
            }
        }
        else if (type === "update") {
            this.logger.log(`update ${JSON.stringify(data)}`)
        }
        else if (type === "error") {
            try {
                this.logger.error(`error ${JSON.stringify(data)}`)
                if ((data["message"]["errno"] === "ECONNREFUSED")||(data["message"]["code"] === "ECONNREFUSED")) {
                    //switching to simulated sessions now...
                    this.logger.warn("will switch to simulator mode - no bsn found")
                    this.switchToSimulator();
                }
            } catch (error) {
                this.logger.error(error)
                this.logger.error(`error ${JSON.stringify(data)}`)
            }
        }
        else {
            this.logger.log(`unknown type ${type} -> ${JSON.stringify(data)}`)
        }
    }

    private switchToSimulator() {
        this.bsnClient.stopMeasurement();
        this.bsnClient.removeCallback();
        this.sessionSimulator.setCallback(this.handleBSNData.bind(this));
        this.sessionSimulator.start();
    }


    simulateSession(newSession: SessionSimulationConfig): SessionConfig {
        //config
        this.mySessionConfig = newSession;
        //session
        this.myActiveSession.date = newSession.start_date;
        this.myActiveSession.timezone = newSession.timezone;
        this.myActiveSession.task = this.mySessionConfig.task;
        this.myActiveSession.events = [];
        //status
        this.mySessionStatus.isActive = true;
        this.mySessionStatus.session = this.myActiveSession.date;

        this.sessionSimulator.setCallback(this.handleBSNData.bind(this));
        this.sessionSimulator.start(newSession.stop_date);

        return this.mySessionConfig;
    }

    private startSession() {
        this.logger.log('starting a new sessions');
        //start measuring
        this.bsnClient.setCallback(this.handleBSNData.bind(this));
        this.bsnClient.startMeasurement(["odr 99", `weight ${this.mySessionConfig.weight}`]);
    }

    public stopSession() {
        if ((this.mySessionStatus.session === null || this.mySessionStatus.session === undefined))
            return this.mySessionStatus;
        this.logger.log(`stopping session ${this.mySessionStatus.session}`);

        if (this.sessionSimulator.isActive() === true) {
            this.sessionSimulator.stop();
        }
        else {
            this.bsnClient.stopMeasurement();
        }
        //add to sessionlist
        if (!(this.myActiveSession === null || this.myActiveSession === undefined) && this.myActiveSession.events.length >= 1) {
            this.mySessionList.push(Object.assign({}, this.myActiveSession));
            this.saveSessionToFile(this.myActiveSession);
        }
        else{
            this.logger.log(`Won't add a empty session`);
        }
        //new activesession
        this.myActiveSession = new CreateBSNDataDTO();
        // reset config
        this.mySessionStatus.isActive = false;
        this.mySessionStatus.session = null;
        this.logger.log(`stopping session done`);

        return this.mySessionStatus;
    }

    public getStatus(): SessionStatus {
        return this.mySessionStatus;
    }
    public getConfig(): SessionConfig {
        return this.mySessionConfig;
    }
    public newSession(newSession: SessionConfig): SessionConfig {
        //config
        this.mySessionConfig = newSession;
        //session
        this.myActiveSession.date = Date.now();
        this.myActiveSession.timezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
        this.myActiveSession.task = this.mySessionConfig.task;
        this.myActiveSession.events = [];
        //status
        this.mySessionStatus.isActive = true;
        this.mySessionStatus.session = this.myActiveSession.date;

        //do the magic with starting stoppping measurement once validated
        // for now there will be just a simulation of data running:
        this.startSession();

        return this.mySessionConfig;
    }

    public getSessions(): number[] {
        return this.mySessionList.map(a => a.date);
    }

    public getSession(id: number): CreateBSNDataDTO {
        const index = this.mySessionList.findIndex(data => data.date === id);
        if (index === -1) {
            this.logger.warn('invalid session id');
            return this.myActiveSession;
        }
        return this.mySessionList[index];
    }

    deleteSession(id: number): number {
        //TODO this is missing a testcase right now!
        const index = this.mySessionList.findIndex(data => data.date === id);
        if (index === -1) {
            this.logger.warn('invalid session id');
            return 0;
        }

        //remove that session from the list
        this.deleteSessionFile(this.mySessionList[index]);
        this.mySessionList.splice(index, 1);
        return id;
    }
}
