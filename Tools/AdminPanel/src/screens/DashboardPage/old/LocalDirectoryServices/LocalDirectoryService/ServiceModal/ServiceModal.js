import React from 'react';
import { Modal, Box, Typography } from '@mui/material';
import DataGrid from '../../../../../../components/DataGrid/DataGrid';

const style = {
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
    width: 800,
    bgcolor: 'background.paper',
    border: '2px solid #000',
    boxShadow: 24,
    p: 4,
};

const ServiceModal = ({ open, handleClose, services }) => {
    const columns = [
        { field: 'name', headerName: 'Service Name', width: 200 },
        { field: 'cpuUsage', headerName: 'CPU Usage', width: 150 },
        { field: 'memoryUsage', headerName: 'Memory Usage', width: 150 },
    ];

    return (
        <Modal
            open={open}
            onClose={handleClose}
            aria-labelledby="modal-modal-title"
            aria-describedby="modal-modal-description"
        >
            <Box sx={style}>
                <Typography id="modal-modal-title" variant="h6" component="h2">
                    Services
                </Typography>
                <DataGrid
                    list={services}
                    columnHeader={columns}
                    title="Service Details"
                />
            </Box>
        </Modal>
    );
};

export default ServiceModal;
