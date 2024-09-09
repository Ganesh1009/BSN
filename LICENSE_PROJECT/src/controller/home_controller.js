'use strict';

const express = require('express');
const axios = require('axios')

const router = express.Router();


const handle_home = async (request, response, next) => {

    const bucket = request.config.db_bucket;

    const process_env = request.config.process_env

    const collection = bucket.defaultCollection();

    (async () => {

        const url = 'http://localhost:8092/' + bucket._name + '/_design/document_id/_view/view_document_id';

        try {
            const view_result = await axios.get(url, {
                auth: {
                    username: process_env.DB_LICENSE_BUCKET_UNAME,
                    password: process_env.DB_LICENSE_BUCKET_PASSWORD
                }
            });

            const results = await Promise.all(view_result.data.rows.map((row) => {
                return collection.get(row.id);
            }));

            response.render('license_info_view', {
                data: results,
                config: request.config,
                req: request
            });

        } catch (error) {
            console.log(`Error: ${error}`);
        }
    })();
};

module.exports = { handle_home };