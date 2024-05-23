import React from 'react';
import Service from './Service/Service';

const LocalDirectoryService = ({ name, cpuUsage, memoryUsage, localServices }) => {
    return (
        <div className="card">
            <div className="card-header">
                {name}
                <ul>
                    <li>CPU usage: {cpuUsage}</li>
                    <li>Memory usage: {memoryUsage}</li>
                </ul>
            </div>
            <div className="card-body">
                {localServices.map((service, index) => (
                    <Service
                        key={index}
                        name={service.name}
                    />
                ))}
            </div>
        </div>
    );
};

export default LocalDirectoryService;
