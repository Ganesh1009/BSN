import { Injectable } from '@nestjs/common';
import { DeviceInfo, FactoryDevice, DeviceSettings } from './device.dto';
import * as os from 'os';
const { exec } = require( 'child_process');

@Injectable()
export class DeviceService {
    private readonly myInfo: DeviceInfo = new FactoryDevice().getDeviceInfo();
    private filename_len : number;
    private accpeted_file_extension : string = ".deb";
    private obtained_file_extension : string;
    private command : string = "dpkg -i ";
    
    private mySettings: DeviceSettings = {
        notificationWithVibration: false,
        notificationOnSmartWatch: false,
        connectedSmartWatchMAC: [
            Math.floor(Math.random() * 255),
            Math.floor(Math.random() * 255),
            Math.floor(Math.random() * 255),
            Math.floor(Math.random() * 255),
            Math.floor(Math.random() * 255),
            Math.floor(Math.random() * 255),
        ],
    };

    public getSettings(): DeviceSettings {
        return this.mySettings;
    }

    public setSettings(newSettings: DeviceSettings): DeviceSettings {
        this.mySettings = newSettings;
        return this.mySettings;
    }

    public getInfo(): DeviceInfo {
        this.myInfo.hostname = os.hostname();
        this.myInfo.kernel = os.release();
        this.myInfo.freeMem = os.freemem();
        this.myInfo.totalMem = os.totalmem();
        this.myInfo.avgLoad = os.loadavg();
        this.myInfo.upTime = os.uptime();
        // TODO
        // get free disk space on ai chip
        // get version numbers frm the different applications
        return this.myInfo;
    }

    public firmwareUpdateStatus( file : Express.Multer.File ) {

        if ( file && file.originalname !== null ) 
        {
            this.filename_len = file.originalname.length;

            this.obtained_file_extension = file.originalname.substring( (this.filename_len - 4), this.filename_len )

            if( this.obtained_file_extension == this.accpeted_file_extension )
            {

                exec( this.command.concat( process.cwd() + "/deb_packages/" + file.originalname ), ( err, stdout, stderr ) => {
                    
                    if ( err )
                    {
                        console.log( "Error: ", stderr );
                        return stderr;
                    }
                    else
                    {
                        console.log( stdout );
                        return "File received successfully and firmware upadate successful. ( Status: 200 )";
                    } 
                });
                    
            }
            else
            {
                return " Bad file received. Pleae ensure that it is .deb file."
            }
        }
    }
}
