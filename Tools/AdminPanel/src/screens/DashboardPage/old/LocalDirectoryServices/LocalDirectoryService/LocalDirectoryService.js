import React, { useState } from 'react';
import { Button } from '@mui/material';
import ServiceModal from './ServiceModal/ServiceModal';

const LocalDirectoryService = ({ name, cpuUsage, memoryUsage, localSessionServices }) => {
    const [open, setOpen] = useState(false);

    const handleOpen = () => setOpen(true);
    const handleClose = () => setOpen(false);

    return (
        <div className="card">
            <div className="card-header">
                {name}
                <ul>
                    <li>CPU usage: {cpuUsage}</li>
                    <li>Memory usage: {memoryUsage}</li>
                </ul>
                <Button variant="outlined" onClick={handleOpen}>
                    View Services
                </Button>
            </div>
            <ServiceModal open={open} handleClose={handleClose} services={localSessionServices} />
        </div>
    );
};

export default LocalDirectoryService;
