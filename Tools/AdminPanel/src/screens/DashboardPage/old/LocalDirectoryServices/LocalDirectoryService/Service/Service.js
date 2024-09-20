import React from 'react';

const Service = ({ name, cpuUsage, memoryUsage }) => {
    return (
        <div className="card">
            <div className="card-header">Service</div>
            <div className="card-body">
                <ul>
                    <li>Name: {name}</li>
                </ul>
            </div>
        </div>
    );
};

export default Service;
