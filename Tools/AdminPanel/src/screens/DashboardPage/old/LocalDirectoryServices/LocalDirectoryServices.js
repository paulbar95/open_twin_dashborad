import React, { useState } from 'react';
import { Button } from '@mui/material';
import DataGrid from '../../../../components/DataGrid/DataGrid';
import ServiceModal from './LocalDirectoryService/ServiceModal/ServiceModal';

const LocalDirectoryServices = ({ localDirectoryServices = [] }) => {
    const [open, setOpen] = useState(false);
    const [selectedServices, setSelectedServices] = useState([]);

    const handleOpenModal = (services) => {
        setSelectedServices(services);
        setOpen(true);
    };
    const handleClose = () => setOpen(false);

    const columns = [
        { field: 'name', headerName: 'Service Name', width: 200 },
        { field: 'cpuUsage', headerName: 'CPU Usage', width: 150 },
        { field: 'memoryUsage', headerName: 'Memory Usage', width: 150 },
    ];

    const rows = localDirectoryServices.map((service, index) => ({
        id: index,
        name: service.name,
        cpuUsage: service.cpuUsage,
        memoryUsage: service.memoryUsage,
        localSessionServices: service.localSessionServices
    }));

    return (
        <div className="card">
            <div className="card-header">Local Directory Services</div>
            <div className="card-body">
                <DataGrid
                    list={rows}
                    columnHeader={columns}
                    title="Local Directory Services"
                    handleOpenModal={handleOpenModal}
                    pageSize={10} // Standardanzahl von Zeilen pro Seite
                />
            </div>
            <ServiceModal open={open} handleClose={handleClose} services={selectedServices} />
        </div>
    );
};

export default LocalDirectoryServices;
