import React from 'react';
import LocalSessionService from './LocalSessionService/LocalSessionService';

const LocalSessionServices = ({ services }) => {
    return (
        <div className="card">
            <div className="card-header">Local Session Services</div>
            <div className="card-body">
                {services.map((service, index) => (
                    <LocalSessionService key={index} serviceNumber={index + 1} sessions={service.sessions} />
                ))}
            </div>
        </div>
    );
};

export default LocalSessionServices;
