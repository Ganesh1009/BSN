import { Test, TestingModule } from '@nestjs/testing';
import { BsnClientService, BsnClientCallback } from './bsn-client.service';

describe('BsnClientService', () => {
    let service: BsnClientService;

    beforeEach(async () => {
        const module: TestingModule = await Test.createTestingModule({
            providers: [BsnClientService],
        }).compile();

        service = module.get<BsnClientService>(BsnClientService);
    });

    it('should be defined', () => {
        expect(service).toBeDefined();
        expect(service.isConnected).toBeDefined();
        expect(service.sendMessage).toBeDefined();
        expect(service.startMeasurement).toBeDefined();
        expect(service.startCalibration).toBeDefined();
        expect(service.stopMeasurement).toBeDefined();
        expect(service.setCallback).toBeDefined();
        expect(service.removeCallback).toBeDefined();
    });

    it('should be connected to a bsn-server', () => {
        /*
    expect(service.isConnected()).toBeFalsy();
    let cb = (type:string, data: object):void => {
        console.log(`${type},${data}`);
    }
    //setcallback
    service.setCallback(cb);
    service.startMeasurement();
    */
    });
});
