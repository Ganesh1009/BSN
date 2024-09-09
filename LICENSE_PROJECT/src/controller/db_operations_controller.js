'use strict'

const handle_db_operations = async (req, res) => {

    var document = {
        'doc_version': parseFloat(1.0),
        'license_id': req.body.license_id,
        'license_type': req.body.license_type,
        'mode': req.body.mode,
        'former_license_id': req.body.former_license_id,
        'invoice_id': parseFloat(req.body.invoice_id),
        'is_upgraded': Boolean(req.body.upgraded),
        'payment_info': {
            'base_price': parseFloat(req.body.base_price),
            'upgrade_price': parseFloat(req.body.upgrade_price)
        },
        'date_info': {
            'initial_release_date': req.body.initial_release_date,
            'upgrade_date': req.body.upgrade_date
        },
        'payment_status_info': {
            'payment_type': req.body.payment_type,
            'status': req.body.status
        },
        'deployment_comment': req.body.deployment_comment,
        'pc_specs': req.body.pc_specs
    };

    const bucket = req.config.db_bucket;

    const collection = bucket.defaultCollection();

    const doc_id = 'meva_' + req.body.mode + '_' + req.body.license_id;

    (async () => {

        try {
            var db_operation_result;
            if (req.body.modification_type == 'Register') {
                db_operation_result = await collection.insert(doc_id, document);
            }
            else if (req.body.modification_type == 'Update') {
                db_operation_result = await collection.upsert(doc_id, document);
            }
            console.log(db_operation_result);
            res.status(204).redirect('/home');
        } catch (error) {
            console.log(`Error while document modification operation : ${error}`);
        }
    })();
};

module.exports = { handle_db_operations };