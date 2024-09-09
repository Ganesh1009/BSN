import { Test, TestingModule } from '@nestjs/testing';
import { SessionsController } from './sessions.controller';
import { SessionsService } from './sessions.service';
import { SessionConfig, FactorySessions } from './sessions.dto';
import { BsnClientService } from '../bsn-client/bsn-client.service';

describe('Sessions Controller', () => {
    let controller: SessionsController;

    beforeEach(async () => {
        const module: TestingModule = await Test.createTestingModule({
            controllers: [SessionsController],
            providers: [SessionsService, BsnClientService],
        }).compile();

        controller = module.get<SessionsController>(SessionsController);
    });

    it('should be defined', () => {
        expect(controller).toBeDefined();
        expect(controller.getStatus).toBeDefined();
        expect(controller.getConfig).toBeDefined();
        expect(controller.getAllSessions).toBeDefined();
        expect(controller.getSession).toBeDefined();
        expect(controller.stopSession).toBeDefined();
    });
    it('should return the right type', () => {
        expect(controller.getStatus()).toHaveProperty('isActive');
        expect(controller.stopSession()).toHaveProperty('isActive');
        expect(controller.getConfig).toBeDefined();
        expect(controller.getAllSessions).toBeDefined();
        expect(controller.getAllSessions()).toContain(406598400000);
        expect(controller.getAllSessions()).toBeInstanceOf(Array);
        expect(controller.getSession).toBeDefined();
        expect(controller.getSession(406598400000)).toBeInstanceOf(Object);
        expect(controller.getSession(406598400000)).toHaveProperty(
            'task',
        );
        expect(controller.getSession(406598400000).task).toEqual(
            'i am just a dummy entry',
        );
        expect(controller.getSession(406598400000)).toHaveProperty('date');
        expect(controller.getSession(406598400000)).toHaveProperty('timezone');
        expect(controller.getSession(406598400000).events).toEqual([]);
        expect(controller.getSession(406598400000)).toHaveProperty('events');
    });
    it('should be writeable and update propely', () => {
        const testSession: SessionConfig = new FactorySessions().createSessionConfig();
        expect(controller.newSession(testSession)).toBeInstanceOf(Object);
        expect(controller.getConfig()).toHaveProperty('height');
        expect(controller.getConfig().height).toEqual(testSession.height);
        expect(controller.getConfig()).toHaveProperty('weight');
        expect(controller.getConfig().weight).toEqual(testSession.weight);
        expect(controller.getConfig()).toHaveProperty('task');
        expect(controller.getConfig().task).toEqual(
            testSession.task,
        );
    });
});
