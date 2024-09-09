import { NestFactory } from '@nestjs/core';
import { ExpressAdapter } from '@nestjs/platform-express';
import { AppModule } from './app.module';
import { ConfigService } from '@nestjs/config';
import { SwaggerModule, DocumentBuilder } from '@nestjs/swagger';
import { DeviceModule } from './device/device.module';
import { SessionsModule } from './sessions/sessions.module';
import { url } from 'inspector';
import { Logger } from '@nestjs/common';
import * as compression from 'compression';

const path = require('path');
const fs = require('fs');
const http = require('http');
const https = require('https');
const express = require('express');

function shouldCompress(req, res) {
    if (req.headers['x-no-compression']) {
        // don't compress responses with this request header
        return false
    }

    // fallback to standard filter function
    return compression.filter(req, res)
}

async function bootstrap() {

    const httpsOptions = {
        key: fs.readFileSync(
            path.resolve(__dirname, './secrets/server_key.pem'),
        ),
        cert: fs.readFileSync(
            path.resolve(__dirname, './secrets/server_cert.pem'),
        ),
        requestCert: true,
        rejectUnauthorized: false,
        ca: [
            fs.readFileSync(
                path.resolve(__dirname, './secrets/server_cert.pem'),
            ),
        ],
    };

    const server = express();
    server.use(compression({ filter: shouldCompress }));

    const app = await NestFactory.create(AppModule, new ExpressAdapter(server));

    const options = new DocumentBuilder()
        .setTitle('bionic ai chip interface')
        .setDescription('description of the bionic ai chip interface')
        .setVersion('1.0')
        .build();

    const document = SwaggerModule.createDocument(app, options, {
        include: [DeviceModule, SessionsModule],
    });

    SwaggerModule.setup('api', app, document);

    app.enableCors();
    await app.init();

    const configService = app.get(ConfigService);
    const http_port = configService.get<number>('BIONIC_HTTP_PORT');
    const https_port = configService.get<number>('BIONIC_HTTPS_PORT');
    const enableHTTP = configService.get('BIONIC_USE_HTTP') === 'true';
    Logger.log(`enableHTTP ${enableHTTP}`);
    Logger.log(`http_port ${http_port}`);
    Logger.log(`https_port ${https_port}`);

    if (enableHTTP === true) {
        http.createServer(server).listen(http_port);
        Logger.log(`HTTP Server listening on ${http_port}`);
    }
    https.createServer(httpsOptions, server).listen(https_port);
    Logger.log(`HTTPS Server listening on ${https_port}`);
}

bootstrap();
