import { Test, TestingModule } from '@nestjs/testing';
import { INestApplication } from '@nestjs/common';
import * as request from 'supertest';
import { AppModule } from './../src/app.module';

describe('AppController (e2e)', () => {
    let app: INestApplication;

    beforeEach(async () => {
        const moduleFixture: TestingModule = await Test.createTestingModule({
            imports: [AppModule],
        }).compile();

        app = moduleFixture.createNestApplication();
        await app.init();
    });

    it('/ (GET)', () => {
        return request(app.getHttpServer())
            .get('/')
            .expect(200)
            .expect('Hello World!');
    });
    it('/device/info (GET)', () => {
        return request(app.getHttpServer())
            .get('/device/info')
            .expect(200);
    });

    it('/device/settings (GET)', () => {
        return request(app.getHttpServer())
            .get('/device/settings')
            .expect(200);
    });

    it('/sessions/status (GET)', () => {
        return request(app.getHttpServer())
            .get('/sessions/status')
            .expect(200);
    });

    it('/sessions/config (GET)', () => {
        return request(app.getHttpServer())
            .get('/sessions/config')
            .expect(200);
    });

    it('/sessions/ (GET)', () => {
        return request(app.getHttpServer())
            .get('/sessions/')
            .expect(200);
    });

    it('/sessions/list/406598400000 (GET)', () => {
        return request(app.getHttpServer())
            .get('/sessions/406598400000')
            .expect(200);
    });
});
