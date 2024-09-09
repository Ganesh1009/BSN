import { Test, TestingModule } from '@nestjs/testing';
import { SessionsService } from './sessions.service';
import { SessionStatus, SessionConfig, FactorySessions } from './sessions.dto';
import { BsnClientService } from '../bsn-client/bsn-client.service';

describe('SessionsService', () => {
    let service: SessionsService;

    beforeEach(async () => {
        const module: TestingModule = await Test.createTestingModule({
            providers: [SessionsService, BsnClientService],
        }).compile();

        service = module.get<SessionsService>(SessionsService);
    });

    it('should be defined', () => {
        expect(service).toBeDefined();
        expect(service.getStatus).toBeDefined();
        expect(service.getConfig).toBeDefined();
        expect(service.getSession).toBeDefined();
        expect(service.deleteSession).toBeDefined();
        expect(service.getSessions).toBeDefined();
        expect(service.newSession).toBeDefined();
        expect(service.stopSession).toBeDefined();
    });

    describe('getStatus', () => {
        it('should return the current status of the sessions', async () => {
            const result: SessionStatus = { isActive: false };
            expect(service.getStatus()).toEqual(result);
        });
    });

    describe('deleteSession', () => {
        it('should remove a session from the list and return the id', async () => {
            const result: number = Date.UTC(1982, 10, 20, 0, 0, 0, 0);
            expect(service.deleteSession(result)).toEqual(result);
            expect(service.deleteSession(3452)).toEqual(0);
            console.log(service.getSessions())
        });
    });

    describe('sessionSimulator', () => {
        it('session should be created and read back properly', () => {
            const testConfig: SessionConfig = new FactorySessions().createSessionConfig();
            expect(service.getStatus().isActive).toBeFalsy();
            expect(service.newSession(testConfig).task).toMatch(
                testConfig.task,
            );

            // jest.useFakeTimers();
            // setTimeout(() => {
            //     expect(service.getStatus().isActive).toBeTruthy();
            //     expect(service.getStatus().session).toBeDefined();
            //     let sessionid = service.getStatus().session;
            //     expect(service.stopSession().isActive).toBeFalsy();
            //     expect(service.getSessions()).toContain(sessionid);
            //     expect(service.getSession(sessionid)).toHaveProperty('events');
            //     expect(service.getSession(sessionid).events).toBeInstanceOf(Array);
            // }, 1500);
            // jest.runAllTimers();
        });
    });
});
