import { Test, TestingModule } from '@nestjs/testing';
import { DeviceController } from './device.controller';
import { DeviceSettings } from './device.dto';
import { DeviceService } from './device.service';

describe('Device Controller', () => {
    let controller: DeviceController;

    beforeEach(async () => {
        const module: TestingModule = await Test.createTestingModule({
            controllers: [DeviceController],
            providers: [DeviceService],
        }).compile();

        controller = module.get<DeviceController>(DeviceController);
    });

    it('should be defined', () => {
        expect(controller).toBeDefined();
        expect(controller.getInfo).toBeDefined();
        expect(controller.getSettings).toBeDefined();
        expect(controller.setSettings).toBeDefined();
    });

    it('should have the corret return types', () => {
        expect(controller.getInfo()).toBeInstanceOf(Object);
        expect(controller.getSettings()).toBeInstanceOf(Object);
    });
    it('should be writeable and update propely', () => {
        const testSettings: DeviceSettings = {
            connectedSmartWatchMAC: [12, 23, 34, 45, 56, 67],
            notificationOnSmartWatch: true,
            notificationWithVibration: false,
        };
        expect(controller.setSettings(testSettings)).toMatchObject(
            testSettings,
        );
    });
});
