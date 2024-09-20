import React from 'react';

const Session = ({ user, accessedLDS }) => {
    return (
        <div className="card">
            <div className="card-header">Session</div>
            <div className="card-body">
                <ul>
                    <li>User: {user}</li>
                    <li>Accessed LDS:
                        <ul>
                            {accessedLDS.map((lds, index) => <li key={index}>{lds}</li>)}
                        </ul>
                    </li>
                </ul>
            </div>
        </div>
    );
};

export default Session;
