import { Test, TestingModule } from '@nestjs/testing';
import { DeviceService } from './device.service';
import { DeviceInfo, FactoryDevice, DeviceSettings } from './device.dto';

describe('DeviceService', () => {
    let service: DeviceService;

    beforeEach(async () => {
        const module: TestingModule = await Test.createTestingModule({
            providers: [DeviceService],
        }).compile();

        service = module.get<DeviceService>(DeviceService);
    });

    it('should be defined', () => {
        expect(service).toBeDefined();
        expect(service.getSettings).toBeDefined();
        expect(service.getInfo).toBeDefined();
    });
    it('should have correct return types', () => {
        const testSettings: DeviceSettings = {
            connectedSmartWatchMAC: [11, 22, 33, 44, 55, 66],
            notificationOnSmartWatch: false,
            notificationWithVibration: true,
        };

        expect(service.setSettings(testSettings)).toMatchObject(testSettings);
        expect(service.getSettings()).toMatchObject(testSettings);

        // const testInfo : DeviceInfo = new DeviceInfoCreator().getDeviceInfo();
        expect(service.getInfo()).toBeInstanceOf(Object);
    });
});
