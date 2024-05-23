import React from 'react';
import LocalDirectoryService from './LocalDirectoryService/LocalDirectoryService';

const LocalDirectoryServices = ({ localDirectoryServices = [] }) => {
    return (
        <div className="card">
            <div className="card-header">Local Directory Services</div>
            <div className="card-body">
                {localDirectoryServices.map((localDirectoryService, index) => (
                    <LocalDirectoryService
                        key={index}
                        name={localDirectoryService.name}
                        cpuUsage={localDirectoryService.cpuUsage}
                        memoryUsage={localDirectoryService.memoryUsage}
                        localServices={localDirectoryService.localServices}
                    />
                ))}
            </div>
        </div>
    );
};

export default LocalDirectoryServices;
